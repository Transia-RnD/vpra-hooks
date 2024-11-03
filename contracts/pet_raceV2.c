#include "hookapi.h"

#define DEBUG 1

// -----------------------------------------------------------------------------

/**
 * 
 * These functions should be moved into the macro.c file
*/

#define DONE(x)\
    accept(SBUF(x),__LINE__);

#define FLIP_ENDIAN_64(n) ((uint64_t)(((n & 0xFFULL) << 56ULL) |             \
                                      ((n & 0xFF00ULL) << 40ULL) |           \
                                      ((n & 0xFF0000ULL) << 24ULL) |         \
                                      ((n & 0xFF000000ULL) << 8ULL) |        \
                                      ((n & 0xFF00000000ULL) >> 8ULL) |      \
                                      ((n & 0xFF0000000000ULL) >> 24ULL) |   \
                                      ((n & 0xFF000000000000ULL) >> 40ULL) | \
                                      ((n & 0xFF00000000000000ULL) >> 56ULL)))


// -----------------------------------------------------------------------------


#define RACE_MODEL_SIZE 77U
#define ENTRY_FEE_OFFSET 53U

uint8_t curr_buffer[20] = {
    0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
    0x00U, 0x00U, 0x50U, 0x58U, 0x50U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U
};

int64_t hook(uint32_t reserved) {
    _g(1,1);
    TRACESTR("pet_race.c: Start.");

    // ACCOUNT: Hook Account
    uint8_t hook_accid[32];
    hook_account(hook_accid + 12, 20);

    // ACCOUNT: Origin Tx Account
    uint8_t otxn_accid[32];
    otxn_field(otxn_accid + 12, 20, sfAccount);

    // races namespace
    uint8_t rc_ns[32];
    uint8_t rcn_key[3] = {'R', 'C', 'N'};
    int64_t rc_size = otxn_param(SBUF(rc_ns), SBUF(rcn_key));

    // race hash
    uint8_t race_hash[32];
    uint8_t rh_key[2] = {'R', 'H'};
    int64_t rh_size = otxn_param(SBUF(race_hash), SBUF(rh_key));

    uint8_t race_model[RACE_MODEL_SIZE];
    int64_t t_exists = 1;
    int64_t rm_exists = state_foreign(SBUF(race_model), SBUF(race_hash), SBUF(rc_ns), hook_accid + 12, 20);

    if (rm_exists == DOESNT_EXIST && rh_size == 32 && rc_size == 32)
    {
        // race model
        uint8_t race_init[RACE_MODEL_SIZE];
        uint8_t rm_key[2] = {'R', 'M'};
        int64_t rm_size = otxn_param(SBUF(race_init), SBUF(rm_key));
        if (rm_size != RACE_MODEL_SIZE)
        {
            rollback(SBUF("pet_race.c: Invalid Race Model: `Size`."), __LINE__);
        }
        state_foreign_set(SBUF(race_init), SBUF(race_hash), SBUF(rc_ns), hook_accid + 12, 20);
    }

    // ADD PET TO RACE
    uint8_t pet_hash[32];
    uint8_t ph_key[2] = {'P', 'H'};
    if (otxn_param(SBUF(pet_hash), SBUF(ph_key)) == 32 && otxn_type() == ttPAYMENT && !BUFFER_EQUAL_20(hook_accid + 12, otxn_accid + 12))
    {
        // validate that you're the owner of the pet/hash
        uint8_t ptid_buff[34];
        util_keylet(SBUF(ptid_buff), KEYLET_UNCHECKED, pet_hash, 32, 0, 0, 0, 0);
        if (slot_set(SBUF(ptid_buff), 1) != 1)
            rollback(SBUF("pet_race.c: Could not load keylet `Pet`"), __LINE__);
        if (slot_subfield(1, sfOwner, 2) != 2)
            rollback(SBUF("pet_race.c: Could not load `Pet` sfOwner"), __LINE__);
        if (slot_subfield(1, sfDigest, 3) != 3)
            rollback(SBUF("pet_race.c: Could not load `Pet` sfDigest"), __LINE__);
        
        uint8_t pet_owner[20];
        slot(SBUF(pet_owner), 2);
        if (!BUFFER_EQUAL_20(otxn_accid + 12, pet_owner))
        {
            rollback(SBUF("pet_race.c: Can't add pet you dont own to race."), __LINE__);
        }

        // VALIDATE: Issuer & Currency
        uint8_t field_value[48];
        int64_t otxn_field_size = otxn_field(field_value, 48,  sfAmount);
        if (!BUFFER_EQUAL_20(field_value + 8U, curr_buffer))
        {
            rollback(SBUF("pet_race.c: Can't add pet invalid currency."), __LINE__);
        }
        if (!BUFFER_EQUAL_20(field_value + 28U, hook_accid + 12))
        {
            rollback(SBUF("pet_race.c: Can't add pet invalid issuer."), __LINE__);
        }
        // VALIDATE: Amount
        int64_t payment_xfl = -INT64_FROM_BUF(field_value);
        if (!float_compare(payment_xfl, race_model + ENTRY_FEE_OFFSET, COMPARE_EQUAL))
        {
            rollback(SBUF("pet_race.c: Can't add pet invalid amount."), __LINE__);
        }
        

        // validate that pet is not already on the race
        uint8_t ph_dump[8];
        if (state_foreign(SBUF(ph_dump), SBUF(pet_hash), SBUF(race_hash), hook_accid + 12, 20) != DOESNT_EXIST)
        {
            rollback(SBUF("pet_race.c: Cannot add pet to race: `Aleady Exists`."), __LINE__);
        }

        int64_t count;
        state_foreign(&count, 8, hook_accid + 12, 20, SBUF(race_hash), hook_accid + 12, 20);

        // validate that there is an open stall
        if (count >= 5)
        {
            rollback(SBUF("pet_race.c: Cannot add pet to race: `No Stalls`."), __LINE__);
        }

        count++;
        
        ASSERT(0 < state_foreign_set(&count, 8, pet_hash, 32, SBUF(race_hash), hook_accid + 12, 20));
        state_foreign_set(&count, 8, hook_accid + 12, 20, SBUF(race_hash), hook_accid + 12, 20);

        // races namespace
        uint8_t rp_ns[32];
        uint8_t rpn_key[3] = {'R', 'P', 'N'};
        otxn_param(SBUF(rp_ns), SBUF(rpn_key));

        // Update Prize Pool
        uint8_t prize_pool[8];
        state_foreign(SBUF(prize_pool), SBUF(rp_ns), SBUF(race_hash), hook_accid + 12, 20);
        int64_t prize_pool_xfl = *((int64_t*)prize_pool);
        int64_t total_pool_xfl = float_sum(prize_pool_xfl, payment_xfl);
        INT64_TO_BUF(prize_pool, FLIP_ENDIAN_64(total_pool_xfl));
        state_foreign_set(SBUF(prize_pool), SBUF(rp_ns), SBUF(race_hash), hook_accid + 12, 20);
        DONE("pet_race.c: Pet added to race.");
    }

    DONE("pet_race.c: Race Created.");
    // unreachable
    return 0;
}