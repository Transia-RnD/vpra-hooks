#include "hookapi.h"

#define DEBUG 1

#define DONE(x) \
    accept(SBUF(x), __LINE__);

#define NOPE(x) \
    rollback(SBUF(x), __LINE__);

#define UINT256_TO_BUF(buf_raw, i)\
{\
    unsigned char* buf = (unsigned char*)buf_raw;\
    *(uint64_t*)(buf + 0) = *(uint64_t*)(i +  0);\
    *(uint64_t*)(buf + 8) = *(uint64_t*)(i +  8);\
    *(uint64_t*)(buf + 16) = *(uint64_t*)(i + 16);\
    *(uint64_t*)(buf + 24) = *(uint64_t*)(i + 24);\
}

#define ACCOUNT_TO_BUF(buf_raw, i)\
{\
    unsigned char* buf = (unsigned char*)buf_raw;\
    *(uint64_t*)(buf + 0) = *(uint64_t*)(i +  0);\
    *(uint64_t*)(buf + 8) = *(uint64_t*)(i +  8);\
    *(uint32_t*)(buf + 16) = *(uint32_t*)(i + 16);\
}

#define INT8_TO_BUF(buf_raw, i)\
{\
    unsigned char* buf = (unsigned char*)buf_raw;\
    buf[0] = (((uint8_t)i) >> 0) & 0xFFUL;\
    if (i < 0) buf[0] |= 0x80U;\
}

#define UINT8_FROM_BUF(buf)\
    (((uint8_t)((buf)[0]) <<  0))

#define LCG_MULTIPLIER 1103515245
#define LCG_INCREMENT 12345
#define LCG_MODULUS (1U << 31)

// Macro to generate a seed from the hash provided by ledger_nonce
#define GENERATE_SEED(hash, hash_size, seed) do { \
    seed = 0; \
    for (int i = 0; GUARD(32), i < sizeof(seed); ++i) { \
        seed ^= ((unsigned int)hash[i] & 0xFF) << (i * 8); \
    } \
} while(0)

// Macro for the LCG function to generate a pseudo-random number
#define LCG_RAND(seed) \
    ((seed) = (LCG_MULTIPLIER * (seed) + LCG_INCREMENT) % LCG_MODULUS)

// Macro to generate a random number in the range [0, count)
#define GENERATE_RANDOM(hash, hash_size, count, random) do { \
    unsigned int seed; \
    GENERATE_SEED(hash, hash_size, seed); \
    LCG_RAND(seed); \
    random = seed % count; \
} while(0)

// -----------------------------------------------------------------------------

/**
 * 
 * These functions should be moved into the macro.c file
*/

// Operations
// 'C' - Create Battle - User creates a battle with a pet
// 'J' - Join Battle - User joins a battle with a pet and type (Attribute | Pet)
// Attribute Battle: Pets battle to "steal" the attribute
// Pet Battle: Pets battle to "steal" the pet
// 'R' - Roll Dice - Each user rolls a dice to determine;
// Defender rolls to determine which attribute to use.
// Attacker rolls to determine which attribute to level up +1.
// 'E' - Battle - Start/End the battle
// When the battle ends;
// The chosen attribute (from dice roll) are compared; the highest wins.
// The winner is credited +1 attribute to their pet
// The loser is debited -1 attribute from their pet
// The battle state joiner is removed so that another attacker can join
// Only one battle can be active at a time
// If the attacker wins, the attacker becomes the defender and owns the location

#define BATTLE_MODEL 139U
#define D_HASH_OFFSET 33U
#define D_ACCOUNT_OFFSET 65U
#define D_ROLL_OFFSET 85U
#define A_HASH_OFFSET 86U
#define A_ACCOUNT_OFFSET 118U
#define A_ROLL_OFFSET 138U

#define PET_MODEL_SIZE 100U
uint8_t pet_ns[32] = {
    0x3B, 0xE1, 0xD4, 0x22, 0xA4, 0x60, 0x85, 0xE1, 0x7F, 0x4A, 0x19, 0xD3, 
    0xED, 0x70, 0x7C, 0x86, 0x69, 0x27, 0xA6, 0xD3, 0x54, 0xA5, 0x62, 0xC4, 
    0x5A, 0x78, 0xF1, 0xF0, 0x3D, 0x7C, 0x41, 0x18
};

// List Of Offsets [speed-6, stamina-7, temperament-8, training-9, health-10, affinity-12]
// Attributes start at 22
uint8_t ATTRB_OFFSET_LIST[] = {0x1CU, 0x1DU, 0x1EU, 0x1FU, 0x20U, 0x22U};

int64_t hook(uint32_t reserved) {
    _g(1,1);

    // ACCOUNT: Hook Account
    uint8_t hook_accid[32];
    hook_account(hook_accid + 12, 20);

    // ACCOUNT: Otxn Account
    uint8_t otxn_accid[32];
    otxn_field(otxn_accid + 12, 20, sfAccount);

    if (BUFFER_EQUAL_20(hook_accid + 12, otxn_accid + 12))
        DONE("pet_battle.c: Outgoing Txn.");

    int64_t tt = otxn_type();

    uint8_t op;
    if (otxn_param(&op, 1, "OP", 2) != 1)
        NOPE("pet_battle.c: Missing `OP` parameter.");
    
    if ((op == 'C' || op == 'J') && tt != ttPAYMENT)
        NOPE("pet_battle.c: Create/Join Battle must be an Payment.");
    
    if ((op == 'R' || op == 'E') && tt != ttINVOKE)
        NOPE("pet_battle.c: Roll/End Battle must be an Invoke.");

    uint8_t battle_hash[32];
    if (otxn_param(SBUF(battle_hash), "BH", 2) != 32)
        NOPE("pet_battle.c: Missing `BH` parameter.");

    uint8_t battle_model[BATTLE_MODEL];
    if (op == 'J' || op == 'R' || op == 'E')
    {
        if (state(SBUF(battle_model), SBUF(battle_hash)) != BATTLE_MODEL)
            NOPE("pet_battle.c: Battle not found.");
    }

    uint8_t pet_id[32];
    uint8_t pet_hash[32];
    if (op == 'C' || op == 'J' || op == 'R')
    {
        if (otxn_param(SBUF(pet_id), "PH", 2) != 32)
            NOPE("pet_battle.c: Missing `PH` parameter.");

        uint8_t urit_buff[34];
        util_keylet(SBUF(urit_buff), KEYLET_UNCHECKED, pet_id, 32, 0, 0, 0, 0);
        if (slot_set(SBUF(urit_buff), 1) != 1)
            NOPE("pet_battle.c: Could not load keylet `URIToken`");
        if (slot_subfield(1, sfOwner, 2) != 2)
            NOPE("pet_battle.c: Could not load `URIToken`");
        if (slot_subfield(1, sfDigest, 3) != 3)
            NOPE("pet_battle.c: Could not load `URIToken`");

        uint8_t urit_owner[20];
        slot(SBUF(urit_owner), 2);
        
        slot(SBUF(pet_hash), 3);

        if (!BUFFER_EQUAL_20(otxn_accid + 12, urit_owner))
            NOPE("pet_battle.c: Invalid Permission to Join Battle.");
    }

    // action
    switch (op)
    {
        case 'C':
        {
            if (state(SBUF(battle_model), SBUF(battle_hash)) != DOESNT_EXIST)
                NOPE("pet_battle.c: Battle already exists.");

            if (otxn_param(SBUF(battle_model), "BM", 2) != BATTLE_MODEL)
                NOPE("pet_battle.c: Missing `BM` parameter.");

            UINT256_TO_BUF(battle_model + D_HASH_OFFSET, pet_hash);
            ACCOUNT_TO_BUF(battle_model + D_ACCOUNT_OFFSET, otxn_accid + 12);
            state_set(SBUF(battle_model), SBUF(battle_hash));
            DONE("pet_battle.c: Battle Created.");
        }

        case 'J':
        {
            UINT256_TO_BUF(battle_model + A_HASH_OFFSET, pet_hash);
            ACCOUNT_TO_BUF(battle_model + A_ACCOUNT_OFFSET, otxn_accid + 12);
            state_set(SBUF(battle_model), SBUF(battle_hash));
            DONE("pet_battle.c: Battle Joined.");
        }

        case 'R':
        {
            int64_t is_attacker = BUFFER_EQUAL_20(otxn_accid + 12, battle_model + A_ACCOUNT_OFFSET);
            int64_t offset = is_attacker ? A_ROLL_OFFSET : D_ROLL_OFFSET;

            if (UINT8_FROM_BUF(battle_model + offset) != 0xFFU)
                NOPE("pet_battle.c: Dice already rolled.");

            uint64_t hash[32];
            ledger_nonce(hash, 32);
            int64_t random_number;
            int64_t sided_die = 5;
            GENERATE_RANDOM(hash, 32, sided_die, random_number);
            INT8_TO_BUF(battle_model + offset, random_number);
            state_set(SBUF(battle_model), SBUF(battle_hash));
            DONE("pet_battle.c: Dice Rolled.");
        }

        case 'E':
        {
            int64_t defender_roll = UINT8_FROM_BUF(battle_model + D_ROLL_OFFSET);
            int64_t attacker_roll = UINT8_FROM_BUF(battle_model + A_ROLL_OFFSET);
            if (defender_roll == 0xFFU || attacker_roll == 0xFFU)
                NOPE("pet_battle.c: Dice not rolled.");

            int64_t attribute_increase =
                attacker_roll == 0 || attacker_roll == 1 || attacker_roll == 2
                    ? 1
                    : 2;

            uint8_t attrb_index = defender_roll;
            int64_t attrb_offset = ATTRB_OFFSET_LIST[attrb_index];

            // Get the attacker pet model
            uint8_t a_pet_model[PET_MODEL_SIZE];
            state_foreign(SBUF(a_pet_model), battle_model + A_HASH_OFFSET, 32, SBUF(pet_ns), hook_accid + 12, 20);
            
            // Get the defender pet model
            uint8_t d_pet_model[PET_MODEL_SIZE];
            state_foreign(SBUF(d_pet_model), battle_model + D_HASH_OFFSET, 32, SBUF(pet_ns), hook_accid + 12, 20);
            
            // Get the attribute values
            uint8_t a_attrb_value = UINT8_FROM_BUF(a_pet_model + attrb_offset);
            uint8_t d_attrb_value = UINT8_FROM_BUF(d_pet_model + attrb_offset);

            // Compare the attribute values
            int64_t attacker_won = a_attrb_value > d_attrb_value ? 1 : 0;
            a_attrb_value = attacker_won ? a_attrb_value + attribute_increase : a_attrb_value - attribute_increase;
            d_attrb_value = attacker_won ? d_attrb_value - attribute_increase : d_attrb_value + attribute_increase;

            // Update the attacker attribute values
            INT8_TO_BUF(a_pet_model + attrb_offset, a_attrb_value);
            state_foreign_set(SBUF(a_pet_model), battle_model + D_HASH_OFFSET, 32, SBUF(pet_ns), hook_accid + 12, 20);
            
            // Update the defender attribute values
            INT8_TO_BUF(d_pet_model + attrb_offset, d_attrb_value);
            state_foreign_set(SBUF(d_pet_model), battle_model + D_HASH_OFFSET, 32, SBUF(pet_ns), hook_accid + 12, 20);

            // Update the battle state
            UINT256_TO_BUF(battle_model + D_HASH_OFFSET, 0);
            ACCOUNT_TO_BUF(battle_model + D_ACCOUNT_OFFSET, 0);
            if (attacker_won == 0)
            {
                // Add the defender to the attacker
                UINT256_TO_BUF(battle_model + A_HASH_OFFSET, battle_model + D_HASH_OFFSET);
                ACCOUNT_TO_BUF(battle_model + A_ACCOUNT_OFFSET, battle_model + D_ACCOUNT_OFFSET);
            }
            // Reset the dice rolls
            INT8_TO_BUF(battle_model + A_ROLL_OFFSET, 0xFFU);
            INT8_TO_BUF(battle_model + D_ROLL_OFFSET, 0xFFU);
            state_set(SBUF(battle_model), SBUF(battle_hash));
            DONE("pet_battle.c: Battle Ended.");
        }

        default:
        {
            NOPE("pet_battle.c: Unknown operation.");
        }
    }
    // unreachable
    return 0;
}