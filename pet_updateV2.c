/**
 * 
 */
 
#include "hookapi.h"

#define INT8_TO_BUF(buf_raw, i)\
{\
    unsigned char* buf = (unsigned char*)buf_raw;\
    buf[0] = (((uint8_t)i) >> 0) & 0xFFUL;\
    if (i < 0) buf[0] |= 0x80U;\
}

#define UINT256_TO_BUF(buf_raw, i)\
{\
    unsigned char* buf = (unsigned char*)buf_raw;\
    *(uint64_t*)(buf + 0) = *(uint64_t*)(i +  0);\
    *(uint64_t*)(buf + 8) = *(uint64_t*)(i +  8);\
    *(uint64_t*)(buf + 16) = *(uint64_t*)(i + 16);\
    *(uint64_t*)(buf + 24) = *(uint64_t*)(i + 24);\
}

// BINARY MODEL
#define MODEL_SIZE 100U
#define NAME_OFFSET 8U // field offset from 0
#define BREEDABLE_OFFSET 43U // field offset from 0
#define PRICE_OFFSET 44U // field offset from 0

int64_t hook(uint32_t reserved ) {

    TRACESTR("pet_update.c: Called.");

    // HookOn: Invoke
    if (otxn_type() != ttINVOKE) // ttINVOKE only
        DONE("pet_update.c: Passing non-Invoke txn. HookOn should be changed to avoid this.");

    // ACCOUNT: Otxn Account
    uint8_t otxn_accid[32];
    otxn_field(otxn_accid + 12, 20, sfAccount);

    uint8_t pet_hash[32];
    uint8_t hash_key[1] = {'P'};
    if (otxn_param(SBUF(pet_hash), SBUF(hash_key)) != 32)
    {
        rollback(SBUF("pet_update.c: Invalid Txn Parameter `P`"), __LINE__);
    }
    TRACEHEX(pet_hash);

    uint8_t breed_price[8];
    uint8_t bp_key[2] = {'B', 'P'};
    if (otxn_param(SBUF(breed_price), SBUF(bp_key)) != 8)
    {
        rollback(SBUF("pet_update.c: Invalid Txn Parameter `BP`"), __LINE__);
    }
    TRACEHEX(breed_price);

    // VALIDATE XFL

    uint8_t pt_buff[34];
    util_keylet(SBUF(pt_buff), KEYLET_UNCHECKED, pet_hash, 32, 0, 0, 0, 0);
    if (slot_set(SBUF(pt_buff), 1) != 1)
        rollback(SBUF("pet_update.c: Could not load keylet `Pet`"), __LINE__);
    if (slot_subfield(1, sfOwner, 2) != 2)
        rollback(SBUF("pet_update.c: Could not load `Pet` sfOwner"), __LINE__);
    if (slot_subfield(1, sfDigest, 3) != 3)
        rollback(SBUF("pet_update.c: Could not load `Pet` sfDigest"), __LINE__);
    
    uint8_t p_owner[20];
    slot(SBUF(p_owner), 2);

    if (!BUFFER_EQUAL_20(otxn_accid + 12, p_owner))
    {
        rollback(SBUF("pet_update.c: Invalid Horse Owner `Account`"), __LINE__);
    }

    uint8_t p_digest[32];
    slot(SBUF(p_digest), 3);

    uint8_t pet_model[MODEL_SIZE];
    ASSERT(0 < state(SBUF(pet_model), p_digest, 32));

    // UPDATE: Name
    uint8_t pn_buff[32];
    uint8_t pn_key[2] = {'P', 'N'};
    if (otxn_param(SBUF(pn_buff), SBUF(pn_key)) == 32)
    {
        UINT256_TO_BUF(pet_model + NAME_OFFSET, pn_buff);
    }

    // UPDATE: Is Breedable
    INT8_TO_BUF(pet_model + BREEDABLE_OFFSET, 1);

    // UPDATE: Breed Price
    pet_model[PRICE_OFFSET + 0] = breed_price[0];
    pet_model[PRICE_OFFSET + 1] = breed_price[1];
    pet_model[PRICE_OFFSET + 2] = breed_price[2];
    pet_model[PRICE_OFFSET + 3] = breed_price[3];
    pet_model[PRICE_OFFSET + 4] = breed_price[4];
    pet_model[PRICE_OFFSET + 5] = breed_price[5];
    pet_model[PRICE_OFFSET + 6] = breed_price[6];
    pet_model[PRICE_OFFSET + 7] = breed_price[7];

    ASSERT(0 < state_set(SBUF(pet_model), p_digest, 32));

    accept(SBUF("pet_update.c: Finished."), __LINE__);

    _g(1,1);
    return 0;
}