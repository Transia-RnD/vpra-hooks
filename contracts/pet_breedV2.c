/**
 *
 */

#include "hookapi.h"

#define ACCOUNT_TO_BUF(buf_raw, i)                       \
    {                                                    \
        unsigned char *buf = (unsigned char *)buf_raw;   \
        *(uint64_t *)(buf + 0) = *(uint64_t *)(i + 0);   \
        *(uint64_t *)(buf + 8) = *(uint64_t *)(i + 8);   \
        *(uint32_t *)(buf + 16) = *(uint32_t *)(i + 16); \
    }

#define UINT112_TO_BUF(buf_raw, i)                       \
    {                                                    \
        unsigned char *buf = (unsigned char *)buf_raw;   \
        *(uint64_t *)(buf + 0) = *(uint64_t *)(i + 0);   \
        buf[8] = i[8];                                  \
        buf[9] = i[9];                                  \
        buf[10] = i[10];                                \
        buf[11] = i[11];                                \
        buf[12] = i[12];                                \
        buf[13] = i[13];                                \
    }

#define UINT256_TO_BUF(buf_raw, i)                       \
    {                                                    \
        unsigned char *buf = (unsigned char *)buf_raw;   \
        *(uint64_t *)(buf + 0) = *(uint64_t *)(i + 0);   \
        *(uint64_t *)(buf + 8) = *(uint64_t *)(i + 8);   \
        *(uint64_t *)(buf + 16) = *(uint64_t *)(i + 16); \
        *(uint64_t *)(buf + 24) = *(uint64_t *)(i + 24); \
    }

#define UINT256_TO_HEXBUF(buf_raw, u256)                   \
    {                                                      \
        const char hex_chars[] = "0123456789ABCDEF";       \
        unsigned char *buf = (unsigned char *)buf_raw;     \
        for (int i = 0; GUARD(32), i < 32; ++i)            \
        {                                                  \
            buf[2 * i] = hex_chars[(u256[i] >> 4) & 0x0F]; \
            buf[2 * i + 1] = hex_chars[u256[i] & 0x0F];    \
        }                                                  \
        buf[2 * 32] = '\0';                                \
    }

#define URI_TO_BUF(buf_raw, uri, len)                            \
    {                                                            \
        unsigned char *buf = (unsigned char *)buf_raw;           \
        for (int i = 0; GUARD(32), i < 32; ++i)                  \
            *(((uint64_t *)buf) + i) = *(((uint64_t *)uri) + i); \
        buf[len + 2] += 0xE1U;                                   \
    }

#define ENCODE_URI(vl, vl_len)               \
    {                                        \
        if (vl_len <= 193)                   \
        {                                    \
            vl[0] = vl_len;                  \
        }                                    \
        else if (vl_len <= 12480)            \
        {                                    \
            vl_len -= 193;                   \
            int byte1 = (vl_len >> 8) + 193; \
            int byte2 = vl_len & 0xFFU;      \
            vl[0] = byte1;                   \
            vl[1] = byte2;                   \
        }                                    \
    }

#define RAND_SEED(seed) ((seed) * 1103515245 + 12345)
#define RAND(seed) (((RAND_SEED(seed)) / 65536) % 32768)

// clang-format off
uint8_t txn[283] =
{
/* size,upto */
/*   3,  0 */   0x12U, 0x00U, 0x00U,                                                            /* tt = Payment */
/*   5,  3*/    0x22U, 0x80U, 0x00U, 0x00U, 0x00U,                                              /* flags = tfCanonical */
/*   5,  8 */   0x24U, 0x00U, 0x00U, 0x00U, 0x00U,                                              /* sequence = 0 */
/*   5, 13 */   0x99U, 0x99U, 0x99U, 0x99U, 0x99U,                                              /* dtag, flipped */
/*   6, 18 */   0x20U, 0x1AU, 0x00U, 0x00U, 0x00U, 0x00U,                                       /* first ledger seq */
/*   6, 24 */   0x20U, 0x1BU, 0x00U, 0x00U, 0x00U, 0x00U,                                       /* last ledger seq */
/*  49, 30 */   0x61U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U,                         /* amount field 9 or 49 bytes */
                0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U,
                0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U,
                0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U,
                0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U,
                0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99,
/*   9, 79 */   0x68U, 0x40U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,                   /* fee      */
/*  35, 88 */   0x73U, 0x21U, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* pubkey   */
/*  22,123 */   0x81U, 0x14U, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                           /* src acc  */
/*  22,145 */   0x83U, 0x14U, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                           /* dst acc  */
/* 116,167 */                                                                                    /* emit details */
/*   0,283 */
};
// clang-format on

// TX BUILDER
#define FLS_OUT (txn + 20U)
#define LLS_OUT (txn + 26U)
#define DTAG_OUT (txn + 14U)
#define AMOUNT_OUT (txn + 30U)
#define FEE_OUT (txn + 80U)
#define HOOK_ACC (txn + 125U)
#define OTX_ACC (txn + 147U)
#define EMIT_OUT (txn + 167U)

// clang-format off
uint8_t rtxn[60000] =
{
/* size,upto */
/*   3,   0 */   0x12U, 0x00U, 0x5FU,                                                           /* tt = Remit       */
/*   5,   3 */   0x22U, 0x80U, 0x00U, 0x00U, 0x00U,                                          /* flags = tfCanonical */
/*   5,   8 */   0x24U, 0x00U, 0x00U, 0x00U, 0x00U,                                                 /* sequence = 0 */
/*   5,  13 */   0x99U, 0x99U, 0x99U, 0x99U, 0x99U,                                                /* dtag, flipped */
/*   6,  18 */   0x20U, 0x1AU, 0x00U, 0x00U, 0x00U, 0x00U,                                      /* first ledger seq */
/*   6,  24 */   0x20U, 0x1BU, 0x00U, 0x00U, 0x00U, 0x00U,                                       /* last ledger seq */
/*   9,  30 */   0x68U, 0x40U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,                         /* fee      */
/*  35,  39 */   0x73U, 0x21U, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,       /* pubkey   */
/*  22,  74 */   0x81U, 0x14U, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                                  /* srcacc  */
/*  22,  96 */   0x83U, 0x14U, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                                  /* dstacc  */
/* 116, 118 */   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,    /* emit detail */
                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*   2, 234 */   0xE0U, 0x5CU, 
/*  34, 236 */   0x50U, 0x15U,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,            /* digest   */
/*   1, 270 */   0x75U,
/*   1, 271 */   0xE1U,
/*   0, 272 */                
};
// clang-format on

// TX BUILDER
#define BYTES_LEN 272U
#define RFLS_OUT (rtxn + 20U)
#define RLLS_OUT (rtxn + 26U)
#define RDTAG_OUT (rtxn + 14U)
#define RFEE_OUT (rtxn + 31U)
#define RHOOK_ACC (rtxn + 76U)
#define ROTX_ACC (rtxn + 98U)
#define REMIT_OUT (rtxn + 118U)
#define RDIGEST_OUT (rtxn + 238U)
#define RURI_OUT (rtxn + 271)
#define URI_LEN 219U // uri length

// clang-format off
#define PREPARE_REMIT_TXN(account_buffer, dest_buffer, uri_buffer, uri_len) do { \ 
    if (otxn_field(RDTAG_OUT, 4, sfSourceTag) == 4) \
        *(DTAG_OUT - 1) = 0x2EU; \
    uint32_t fls = (uint32_t)ledger_seq() + 1; \ 
    *((uint32_t *)(RFLS_OUT)) = FLIP_ENDIAN(fls); \ 
    uint32_t lls = fls + 4; \ 
    *((uint32_t *)(RLLS_OUT)) = FLIP_ENDIAN(lls); \
    ACCOUNT_TO_BUF(RHOOK_ACC, account_buffer); \ 
    ACCOUNT_TO_BUF(ROTX_ACC, dest_buffer); \ 
    etxn_details(REMIT_OUT, 116U); \ 
    URI_TO_BUF(RURI_OUT, uri_buffer, uri_len); \
    int64_t fee = etxn_fee_base(rtxn, BYTES_LEN + uri_len + 2); \ 
    uint8_t *b = RFEE_OUT; \ 
    *b++ = 0b01000000 + ((fee >> 56) & 0b00111111); \ 
    *b++ = (fee >> 48) & 0xFFU; \ 
    *b++ = (fee >> 40) & 0xFFU; \ 
    *b++ = (fee >> 32) & 0xFFU; \ 
    *b++ = (fee >> 24) & 0xFFU; \ 
    *b++ = (fee >> 16) & 0xFFU; \ 
    *b++ = (fee >> 8) & 0xFFU; \ 
    *b++ = (fee >> 0) & 0xFFU; \
} while(0)
// clang-format on

#define FLIP_ENDIAN_64(n) ((uint64_t)(((n & 0xFFULL) << 56ULL) |             \
                                      ((n & 0xFF00ULL) << 40ULL) |           \
                                      ((n & 0xFF0000ULL) << 24ULL) |         \
                                      ((n & 0xFF000000ULL) << 8ULL) |        \
                                      ((n & 0xFF00000000ULL) >> 8ULL) |      \
                                      ((n & 0xFF0000000000ULL) >> 24ULL) |   \
                                      ((n & 0xFF000000000000ULL) >> 40ULL) | \
                                      ((n & 0xFF00000000000000ULL) >> 56ULL)))

// BINARY MODEL
#define MODEL_SIZE 100U
#define ID_OFFSET 0U         // field offset from 0
#define NAME_OFFSET 8U       // field offset from 0
#define ATTRIB_OFFSET 22U    // field offset from 0
#define BREEDABLE_OFFSET 43U // field offset from 0
#define PRICE_OFFSET 44U     // field offset from 0
#define TOKENID_OFFSET 52U   // field offset from 0
#define WINS_OFFSET 84U      // field offset from 0
#define TOTAL_OFFSET 92U     // field offset from 0

// Male Hash: Owner
// Female Hash: Marketplace
// Amount: $10

// Female Hash: Owner
// Male Hash: Marketplace
// Amount: $10

int64_t hook(uint32_t reserved)
{

    // ACCOUNT: Hook Account
    uint8_t hook_acct[32];
    hook_account(hook_acct + 12, 20);

    // HookOn: Invoke
    if (otxn_type() != ttPAYMENT) // ttPAYMENT only
        DONE("pet_breed.c: Passing non-Payment txn. HookOn should be changed to avoid this.");

    // ACCOUNT: Otxn Account
    uint8_t otxn_accid[32];
    otxn_field(otxn_accid + 12, 20, sfAccount);

    uint8_t mh_hash[32];
    uint8_t hm_key[2] = {'P', 'M'};
    if (otxn_param(SBUF(mh_hash), SBUF(hm_key)) != 32)
    {
        rollback(SBUF("pet_breed.c: Invalid Txn Parameter `PM`"), __LINE__);
    }

    uint8_t fh_hash[32];
    uint8_t hf_key[2] = {'P', 'F'};
    if (otxn_param(SBUF(fh_hash), SBUF(hf_key)) != 32)
    {
        rollback(SBUF("pet_breed.c: Invalid Txn Parameter `PF`"), __LINE__);
    }

    uint8_t hn_buff[14];
    uint8_t hn_key[2] = {'P', 'N'};
    if (otxn_param(SBUF(hn_buff), SBUF(hn_key)) != 14)
    {
        rollback(SBUF("pet_breed.c: Invalid Txn Parameter `PN`"), __LINE__);
    }

    uint8_t mpt_buff[34];
    util_keylet(SBUF(mpt_buff), KEYLET_UNCHECKED, mh_hash, 32, 0, 0, 0, 0);
    if (slot_set(SBUF(mpt_buff), 1) != 1)
        rollback(SBUF("pet_breed.c: Could not load keylet `Male`"), __LINE__);
    if (slot_subfield(1, sfOwner, 2) != 2)
        rollback(SBUF("pet_breed.c: Could not load `Male` sfOwner"), __LINE__);
    if (slot_subfield(1, sfDigest, 3) != 3)
        rollback(SBUF("pet_breed.c: Could not load `Male` sfDigest"), __LINE__);

    uint8_t mp_owner[20];
    slot(SBUF(mp_owner), 2);

    uint8_t mp_digest[32];
    slot(SBUF(mp_digest), 3);

    uint8_t mh_model[MODEL_SIZE];
    ASSERT(0 < state(SBUF(mh_model), SBUF(mp_digest)));

    uint8_t fpt_buff[34];
    util_keylet(SBUF(fpt_buff), KEYLET_UNCHECKED, fh_hash, 32, 0, 0, 0, 0);
    if (slot_set(SBUF(fpt_buff), 4) != 4)
        rollback(SBUF("pet_breed.c: Could not load keylet `Female`"), __LINE__);
    if (slot_subfield(4, sfOwner, 5) != 5)
        rollback(SBUF("pet_breed.c: Could not load `Female` sfOwner"), __LINE__);
    if (slot_subfield(4, sfDigest, 6) != 6)
        rollback(SBUF("pet_breed.c: Could not load `Female` sfDigest"), __LINE__);

    uint8_t fp_owner[20];
    slot(SBUF(fp_owner), 5);

    uint8_t fp_digest[32];
    slot(SBUF(fp_digest), 6);

    uint8_t fh_model[MODEL_SIZE];
    ASSERT(0 < state(SBUF(fh_model), SBUF(fp_digest)));

    // OWNER MODE
    int64_t is_f_owner = BUFFER_EQUAL_20(otxn_accid + 12, fp_owner);
    int64_t is_m_owner = BUFFER_EQUAL_20(otxn_accid + 12, mp_owner);
    if (!is_f_owner && !is_m_owner)
    {
        rollback(SBUF("pet_breed.c: Invalid Pet Owner `Account`"), __LINE__);
    }

    if (BUFFER_EQUAL_20(mp_owner, fp_owner))
    {
        rollback(SBUF("pet_breed.c: You cannot breed your own pets"), __LINE__);
    }

    // VALIDATE MALE & FEMALE
    uint8_t field_value[8];
    int64_t otxn_field_size = otxn_field(field_value, 8, sfAmount);
    int64_t payment_drops = AMOUNT_TO_DROPS(field_value);
    int64_t pet_xfl = FLIP_ENDIAN_64(UINT64_FROM_BUF(is_f_owner ? mh_model + PRICE_OFFSET : fh_model + PRICE_OFFSET));
    if (float_int(pet_xfl, 6, 1) != payment_drops)
    {
        rollback(SBUF("pet_breed.c: Invalid Payment `Amount`"), __LINE__);
    }

    int64_t seq = ledger_seq();
    int64_t ts = ledger_last_time();

    unsigned int seed = seq + otxn_accid + 12U;
    seed ^= ts;

    uint8_t mh_gender = mh_model[ATTRIB_OFFSET + 0];
    uint8_t fh_gender = fh_model[ATTRIB_OFFSET + 0];
    uint8_t mh_breed = mh_model[ATTRIB_OFFSET + 2];
    uint8_t fh_breed = fh_model[ATTRIB_OFFSET + 2];
    uint8_t mh_size = mh_model[ATTRIB_OFFSET + 3];
    uint8_t fh_size = fh_model[ATTRIB_OFFSET + 3];
    uint8_t mh_body = mh_model[ATTRIB_OFFSET + 4];
    uint8_t fh_body = fh_model[ATTRIB_OFFSET + 4];
    uint8_t mh_hooves = mh_model[ATTRIB_OFFSET + 5];
    uint8_t fh_hooves = fh_model[ATTRIB_OFFSET + 5];
    uint8_t mh_speed = mh_model[ATTRIB_OFFSET + 6];
    uint8_t fh_speed = fh_model[ATTRIB_OFFSET + 6];
    uint8_t mh_stamina = mh_model[ATTRIB_OFFSET + 7];
    uint8_t fh_stamina = fh_model[ATTRIB_OFFSET + 7];
    uint8_t mh_temperament = mh_model[ATTRIB_OFFSET + 8];
    uint8_t fh_temperament = fh_model[ATTRIB_OFFSET + 8];
    uint8_t mh_training = mh_model[ATTRIB_OFFSET + 9];
    uint8_t fh_training = fh_model[ATTRIB_OFFSET + 9];
    uint8_t mh_health = mh_model[ATTRIB_OFFSET + 10];
    uint8_t fh_health = fh_model[ATTRIB_OFFSET + 10];
    uint8_t mh_lifespan = mh_model[ATTRIB_OFFSET + 11];
    uint8_t fh_lifespan = fh_model[ATTRIB_OFFSET + 11];

    int64_t count;
    state(&count, 8, hook_acct + 12, 20);

    uint8_t pet_model[MODEL_SIZE];
    // id
    INT64_TO_BUF(pet_model + ID_OFFSET, count)
    // name
    UINT112_TO_BUF(pet_model + NAME_OFFSET, hn_buff);
    // gender
    pet_model[ATTRIB_OFFSET + 0] = RAND(seed) % 5 <= 3 ? mh_gender : fh_gender;
    seed ^= ts;
    // age
    pet_model[ATTRIB_OFFSET + 1] = 0x00;
    // breed
    pet_model[ATTRIB_OFFSET + 2] = RAND(seed) % 5 <= 3 ? mh_breed : fh_breed;
    seed ^= ts;
    // size
    pet_model[ATTRIB_OFFSET + 3] = (mh_size + fh_size) / 2;
    // body
    pet_model[ATTRIB_OFFSET + 4] = RAND(seed) % 5 <= 3 ? mh_body : fh_body;
    seed ^= ts;
    // hooves
    pet_model[ATTRIB_OFFSET + 5] = RAND(seed) % 5 <= 3 ? mh_hooves : fh_hooves;
    seed ^= ts;
    // speed
    pet_model[ATTRIB_OFFSET + 6] = RAND(seed) % 5 <= 3 ? mh_speed : fh_speed;
    seed ^= ts;
    // stamina
    pet_model[ATTRIB_OFFSET + 7] = RAND(seed) % 5 <= 3 ? mh_stamina : fh_stamina;
    seed ^= ts;
    // temperament
    pet_model[ATTRIB_OFFSET + 8] = RAND(seed) % 5 <= 3 ? mh_temperament : fh_temperament;
    seed ^= ts;
    // training
    pet_model[ATTRIB_OFFSET + 9] = RAND(seed) % 5 <= 3 ? mh_training : fh_training;
    seed ^= ts;
    // health
    pet_model[ATTRIB_OFFSET + 10] = RAND(seed) % 5 <= 3 ? mh_health : fh_health;
    seed ^= ts;
    // lifespan
    pet_model[ATTRIB_OFFSET + 11] = RAND(seed) % 5 <= 3 ? mh_lifespan : fh_lifespan;
    // affinity
    pet_model[ATTRIB_OFFSET + 12] = RAND(seed) % 100;
    // morale
    pet_model[ATTRIB_OFFSET + 13] = 0x00;
    // is breedable
    pet_model[BREEDABLE_OFFSET] = 0x00;
    // price
    pet_model[PRICE_OFFSET] = 0x00;

    // build uri
    INT64_TO_BUF(otxn_accid, count);
    uint8_t hash_out[32];
    util_sha512h(hash_out, 32, SBUF(otxn_accid));

    uint8_t uri_buffer[2 + 219];
    char url[] = "https://api.vpra.app/v1/metadata?account=";
    char address[] = "rUYiTLYpN8M4xLhtRD9HQZFwqZ4WaKJc89";
    char _ns[] = "&namespace=";
    char _key[] = "&key=";

    for (int i = 0; GUARD(41), i < 41; ++i)
    {
        uri_buffer[i + 2] = url[i];
    }
    for (int i = 0; GUARD(34), i < 34; ++i)
    {
        uri_buffer[i + 41 + 2] = address[i];
    }
    for (int i = 0; GUARD(11), i < 11; ++i)
    {
        uri_buffer[i + 75 + 2] = _ns[i];
    }

    uint8_t ns[32] = {
        0x3B, 0xE1, 0xD4, 0x22, 0xA4, 0x60, 0x85, 0xE1,
        0x7F, 0x4A, 0x19, 0xD3, 0xED, 0x70, 0x7C, 0x86,
        0x69, 0x27, 0xA6, 0xD3, 0x54, 0xA5, 0x62, 0xC4,
        0x5A, 0x78, 0xF1, 0xF0, 0x3D, 0x7C, 0x41, 0x18};
    UINT256_TO_HEXBUF(uri_buffer + 86 + 2, ns);
    for (int i = 0; GUARD(5), i < 5; ++i)
    {
        uri_buffer[i + 150 + 2] = _key[i];
    }
    UINT256_TO_HEXBUF(uri_buffer + 155 + 2, hash_out);
    UINT256_TO_BUF(RDIGEST_OUT, hash_out);

    uint8_t tokenid_buffer[2 + 20 + URI_LEN];
    tokenid_buffer[0] = 0x00U;
    tokenid_buffer[1] = 0x55U;
    ACCOUNT_TO_BUF(tokenid_buffer + 2, hook_acct + 12);
    for (int i = 0; GUARD(URI_LEN), i < URI_LEN; ++i)
    {
        tokenid_buffer[i + 2 + 20] = uri_buffer[i + 2];
    }
    uint8_t tokenid_out[32];
    util_sha512h(tokenid_out, 32, SBUF(tokenid_buffer));

    // token id
    UINT256_TO_BUF(pet_model + TOKENID_OFFSET, tokenid_out);
    // wins
    pet_model[WINS_OFFSET] = 0x00;
    // total
    pet_model[TOTAL_OFFSET] = 0x00;

    // TXN: PREPARE: Init
    etxn_reserve(2);
    uint64_t vl_len = URI_LEN;
    ENCODE_URI(uri_buffer, vl_len);
    PREPARE_REMIT_TXN(hook_acct + 12, otxn_accid + 12, uri_buffer, URI_LEN);

    // TXN: Emit/Send Txn
    uint8_t remithash[32];
    int64_t remit_result = emit(SBUF(remithash), rtxn, BYTES_LEN + URI_LEN + 2);
    if (remit_result < 0)
    {
        rollback(SBUF("pet_breed.c: Emit Failure."), __LINE__);
    }

    // TXN: Account
    ACCOUNT_TO_BUF(HOOK_ACC, hook_acct + 12);

    // TXN: Destination
    ACCOUNT_TO_BUF(OTX_ACC, is_f_owner ? mp_owner : fp_owner);

    // TXN PREPARE: FirstLedgerSequence
    uint32_t fls = (uint32_t)ledger_seq() + 1;
    *((uint32_t *)(FLS_OUT)) = FLIP_ENDIAN(fls);

    // TXN PREPARE: LastLedgerSequense
    uint32_t lls = fls + 4;
    *((uint32_t *)(LLS_OUT)) = FLIP_ENDIAN(lls);

    // TXN PREPARE: Amount
    uint64_t drops = payment_drops;
    uint8_t *b = AMOUNT_OUT + 1;
    *b++ = 0b01000000 + ((drops >> 56) & 0b00111111);
    *b++ = (drops >> 48) & 0xFFU;
    *b++ = (drops >> 40) & 0xFFU;
    *b++ = (drops >> 32) & 0xFFU;
    *b++ = (drops >> 24) & 0xFFU;
    *b++ = (drops >> 16) & 0xFFU;
    *b++ = (drops >> 8) & 0xFFU;
    *b++ = (drops >> 0) & 0xFFU;

    // TXN PREPARE: Dest Tag <- Source Tag
    if (otxn_field(DTAG_OUT, 4, sfSourceTag) == 4)
        *(DTAG_OUT - 1) = 0x2EU;

    // TXN PREPARE: Emit Metadata
    etxn_details(EMIT_OUT, 116U);

    // TXN PREPARE: Fee
    {
        int64_t fee = etxn_fee_base(SBUF(txn));
        uint8_t *b = FEE_OUT;
        *b++ = 0b01000000 + ((fee >> 56) & 0b00111111);
        *b++ = (fee >> 48) & 0xFFU;
        *b++ = (fee >> 40) & 0xFFU;
        *b++ = (fee >> 32) & 0xFFU;
        *b++ = (fee >> 24) & 0xFFU;
        *b++ = (fee >> 16) & 0xFFU;
        *b++ = (fee >> 8) & 0xFFU;
        *b++ = (fee >> 0) & 0xFFU;
    }

    // TXN: Emit/Send Txn
    uint8_t emithash[32];
    int64_t emit_result = emit(SBUF(emithash), SBUF(txn));
    if (emit_result < 0)
    {
        rollback(SBUF("pet_breed.c: Emit Failure."), __LINE__);
    }

    ASSERT(0 < state_set(SBUF(pet_model), hash_out, 32));

    count++;
    state_set(&count, 8, hook_acct + 12, 20);

    accept(SBUF("pet_breed.c: Emit Success."), __LINE__);

    _g(1, 1);
    return 0;
}