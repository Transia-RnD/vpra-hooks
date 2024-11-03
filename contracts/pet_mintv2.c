/**
 * 
 */
 
#include "hookapi.h"

#define ACCOUNT_TO_BUF(buf_raw, i)\
{\
    unsigned char* buf = (unsigned char*)buf_raw;\
    *(uint64_t*)(buf + 0) = *(uint64_t*)(i +  0);\
    *(uint64_t*)(buf + 8) = *(uint64_t*)(i +  8);\
    *(uint32_t*)(buf + 16) = *(uint32_t*)(i + 16);\
}

#define URI_TO_BUF(buf_raw, uri, len)\
{\
    unsigned char* buf = (unsigned char*)buf_raw;\
    for (int i = 0; GUARD(32), i < 32; ++i) \
        *(((uint64_t*)buf) + i) = *(((uint64_t*)uri) + i); \
    buf[len + 1] += 0xE1U; \
}

#define UINT256_TO_BUF(buf_raw, i)                       \
{                                                    \
    unsigned char *buf = (unsigned char *)buf_raw;   \
    *(uint64_t *)(buf + 0) = *(uint64_t *)(i + 0);   \
    *(uint64_t *)(buf + 8) = *(uint64_t *)(i + 8);   \
    *(uint64_t *)(buf + 16) = *(uint64_t *)(i + 16); \
    *(uint64_t *)(buf + 24) = *(uint64_t *)(i + 24); \
}

#define UINT256_TO_HEXBUF(buf_raw, u256)                \
{                                                    \
    const char hex_chars[] = "0123456789ABCDEF";     \
    unsigned char *buf = (unsigned char *)buf_raw;   \
    for (int i = 0; GUARD(32), i < 32; ++i) {        \
        buf[2 * i] = hex_chars[(u256[i] >> 4) & 0x0F];  \
        buf[2 * i + 1] = hex_chars[u256[i] & 0x0F];     \
    }                                                \
    buf[2 * 32] = '\0';                              \
}

#define URI_TO_BUF(buf_raw, uri, len)\
{\
    unsigned char* buf = (unsigned char*)buf_raw;\
    for (int i = 0; GUARD(32), i < 32; ++i) \
        *(((uint64_t*)buf) + i) = *(((uint64_t*)uri) + i); \
    buf[len + 2] += 0xE1U; \
}

#define ENCODE_URI(vl, vl_len)\
    {\
        if (vl_len <= 193) \
        {\
            vl[0] = vl_len;\
        }\
        else if (vl_len <= 12480) \
        {\
            vl_len -= 193;\
            int byte1 = (vl_len >> 8) + 193;\
            int byte2 = vl_len & 0xFFU;\
            vl[0] = byte1;\
            vl[1] = byte2;\
        }\
        else if (vl_len > 12480) \
        {\
            vl_len -= 193;\
            int byte1 = (vl_len >> 8) + 193;\
            int byte2 = vl_len & 0xFFU;\
            vl[0] = byte1;\
            vl[1] = byte2;\
        }\
    }

// clang-format off
uint8_t txn[60000] =
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
/*   2, 234 */   0xF0U, 0x5CU,                                                               /* lead-in amount array */
/*   2, 236 */   0xE0U, 0x5BU,                                                              /* lead-in amount entry A*/
/*  49, 238 */   0x61U,0,0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                                                /* amount A */
                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*   3, 287 */   0xE1U, 0xF1U,                                                 /* lead out, may also appear at end of A */
/*   2, 290 */   0xE0U, 0x5CU, 
/*  34, 292 */   0x50U, 0x15U,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,            /* digest   */
/*   1, 326 */   0x75U,
/*   1, 327 */   0xE1U,
/*   0, 328 */                
};
// clang-format on

// TX BUILDER
#define BYTES_LEN 327U
#define FLS_OUT (txn + 20U)
#define LLS_OUT (txn + 26U)
#define DTAG_OUT (txn + 14U)
#define FEE_OUT (txn + 31U)
#define HOOK_ACC (txn + 76U)
#define OTX_ACC (txn + 98U)
#define EMIT_OUT (txn + 118U)
#define AMOUNT_OUT (txn + 238U)
#define DIGEST_OUT (txn + 293U)
#define URI_OUT (txn + 326)
#define URI_LEN 219U // uri length

// clang-format off
#define PREPARE_REMIT_TXN(account_buffer, dest_buffer, uri_buffer, uri_len, currency) do { \
    if (otxn_field(DTAG_OUT, 4, sfSourceTag) == 4) \
        *(DTAG_OUT - 1) = 0x2EU; \
    uint32_t fls = (uint32_t)ledger_seq() + 1; \
    *((uint32_t *)(FLS_OUT)) = FLIP_ENDIAN(fls); \
    uint32_t lls = fls + 4; \
    *((uint32_t *)(LLS_OUT)) = FLIP_ENDIAN(lls); \
    ACCOUNT_TO_BUF(HOOK_ACC, account_buffer); \
    ACCOUNT_TO_BUF(OTX_ACC, dest_buffer); \
    etxn_details(EMIT_OUT, 116U); \
    uint64_t amount_xlf = 6143909891733356544; \
    float_sto(AMOUNT_OUT, 49, currency, 20, account_buffer, 20, amount_xlf, sfAmount); \
    URI_TO_BUF(URI_OUT, uri_buffer, uri_len); \
    TRACEHEX(txn); \
    int64_t fee = etxn_fee_base(txn, BYTES_LEN + uri_len + 2); \
    uint8_t *b = FEE_OUT; \
    *b++ = 0b01000000 + ((fee >> 56) & 0b00111111); \
    *b++ = (fee >> 48) & 0xFFU; \
    *b++ = (fee >> 40) & 0xFFU; \
    *b++ = (fee >> 32) & 0xFFU; \
    *b++ = (fee >> 24) & 0xFFU; \
    *b++ = (fee >> 16) & 0xFFU; \
    *b++ = (fee >> 8) & 0xFFU; \
    *b++ = (fee >> 0) & 0xFFU; \
    TRACEHEX(txn); \
} while(0) 
// clang-format on

#define RAND_SEED(seed) ((seed) * 1103515245 + 12345)
#define RAND(seed) (((RAND_SEED(seed)) / 65536) % 32768)

uint8_t gender_options[2] = {0x00, 0x01};
uint8_t breed_options[4] = {0x00, 0x01, 0x02, 0x03};
uint8_t base_options[10] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A};
uint8_t speed_options[] = {0x37, 0x3C, 0x41, 0x46, 0x4B, 0x50, 0x55, 0x5A, 0x5F, 0x64};
uint8_t lifespan_options[] = {0x0A, 0x0F, 0x14, 0x19, 0x1E, 0x23, 0x28, 0x2D, 0x32, 0x37};

#define MODEL_SIZE 100U
#define ID_OFFSET 0U // field offset from 0
#define NAME_OFFSET 8U // field offset from 0
#define ATTRIB_OFFSET 22U // field offset from 0
#define BREEDABLE_OFFSET 43U // field offset from 0
#define PRICE_OFFSET 44U // field offset from 0
#define TOKENID_OFFSET 52U // field offset from 0
#define WINS_OFFSET 84U // field offset from 0
#define TOTAL_OFFSET 92U // field offset from 0

uint8_t txn_currency[20] = {
    0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
    0x00U, 0x00U, 0x50U, 0x58U, 0x50U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U
};

#define DEFAULT_DELAY 7200
uint8_t timer_ns[32] = {
    0xF6U, 0x9DU, 0xE0U, 0xCDU, 0x9DU, 0xD6U, 0x22U, 0x99U, 0x01U, 0x2EU, 
    0xBDU, 0x86U, 0x5EU, 0x5BU, 0x81U, 0x28U, 0x2BU, 0xB0U, 0x45U, 0x93U, 
    0xB6U, 0x97U, 0x28U, 0xC6U, 0x53U, 0xF5U, 0x68U, 0x08U, 0x2EU, 0x5BU, 
    0x79U, 0x1AU
};

#define ttSET_HOOK 22U

uint8_t msg_buf[] = "pet_mint.c: You must wait 0000000 seconds";
int64_t hook(uint32_t reserved ) {

    TRACESTR("pet_mint.c: Called.");

    // ACCOUNT: Hook Account
    uint8_t hook_acct[32];
    hook_account(hook_acct + 12, 20);

    // ACCOUNT: OTXN Account
    uint8_t otxn_accid[32];
    otxn_field(otxn_accid + 12, 20, sfAccount);

    // HOOK PARAM: Amount
    uint8_t mp_buffer[8];
    uint8_t mp_key[2] = {'M', 'P'};
    hook_param(SBUF(mp_buffer), SBUF(mp_key));
    int64_t mint_price_xfl = *((int64_t*)mp_buffer);

    // VALIDATE: Amount
    uint8_t field_value[8];
    int64_t otxn_field_size = otxn_field(field_value, 8,  sfAmount);
    int64_t payment = AMOUNT_TO_DROPS(field_value);
    if (!float_compare(float_int(mint_price_xfl, 6, 1), payment, COMPARE_EQUAL))
    {
        rollback(SBUF("pet_mint.c: Invalid Payment `Amount`"), __LINE__);
    }

    int64_t seq = ledger_seq();
    int64_t ts = ledger_last_time();
    
    int64_t time;
    if (state_foreign(&time, 8, otxn_accid + 12, 20, SBUF(timer_ns), hook_acct + 12, 20) == 8)
    {
        int64_t time_elapsed = ts - time;
        if (time_elapsed < DEFAULT_DELAY)
        {
            time_elapsed = DEFAULT_DELAY - time_elapsed;
            msg_buf[26] += (time_elapsed / 1000000) % 10;
            msg_buf[27] += (time_elapsed /  100000) % 10;
            msg_buf[28] += (time_elapsed /   10000) % 10;
            msg_buf[29] += (time_elapsed /    1000) % 10;
            msg_buf[30] += (time_elapsed /     100) % 10;
            msg_buf[31] += (time_elapsed /      10) % 10;
            msg_buf[32] += (time_elapsed          ) % 10;
            rollback(SBUF(msg_buf), __LINE__);
        }
    }

    uint8_t seed_data[20 + 8 + 8];
#define ACCT_OUT (seed_data + 0U)
    seed_data[20] = seq;
    seed_data[28] = ts;
    ACCOUNT_TO_BUF(ACCT_OUT, otxn_accid);

    uint8_t hash[32];
    util_sha512h(SBUF(hash), SBUF(seed_data));
    TRACEHEX(hash);

    unsigned int seed = 0;
    for (int i = 0; GUARD(32), i < sizeof(seed); ++i) {
        seed |= (unsigned int)hash[i] << (i * 8);
    }

    uint8_t hn_buff[32];
    uint8_t hm_key[2] = {'P', 'N'};
    if (otxn_param(SBUF(hn_buff), SBUF(hm_key)) != 32)
    {
        rollback(SBUF("pet_mint.c: Invalid Txn Parameter `PN`"), __LINE__);
    }

    int64_t count;
    state(&count, 8, hook_acct + 12, 20);

    uint8_t pet_model[MODEL_SIZE];

    // id
    INT64_TO_BUF(pet_model + ID_OFFSET, count)
    // name
    UINT256_TO_BUF(pet_model + NAME_OFFSET, hn_buff);
    // gender
    seed = RAND_SEED(seed);
    pet_model[ATTRIB_OFFSET + 0] = gender_options[RAND(seed) % 2];
    // age
    pet_model[ATTRIB_OFFSET + 1] = 0x00;
    // breed
    seed = RAND_SEED(seed);
    pet_model[ATTRIB_OFFSET + 2] = breed_options[RAND(seed) % 4];
    // size
    seed = RAND_SEED(seed);
    pet_model[ATTRIB_OFFSET + 3] = base_options[RAND(seed) % 10];
    // body
    seed = RAND_SEED(seed);
    pet_model[ATTRIB_OFFSET + 4] = base_options[RAND(seed) % 10];
    // hooves
    seed = RAND_SEED(seed);
    pet_model[ATTRIB_OFFSET + 5] = base_options[RAND(seed) % 10];
    // speed
    seed = RAND_SEED(seed);
    pet_model[ATTRIB_OFFSET + 6] = speed_options[RAND(seed) % 10];
    // stamina
    seed = RAND_SEED(seed);
    pet_model[ATTRIB_OFFSET + 7] = base_options[RAND(seed) % 10];
    // temperament
    seed = RAND_SEED(seed);
    pet_model[ATTRIB_OFFSET + 8] = base_options[RAND(seed) % 10];
    // training
    pet_model[ATTRIB_OFFSET + 9] = base_options[RAND(seed) % 10];
    // health
    pet_model[ATTRIB_OFFSET + 10] = base_options[RAND(seed) % 10];
    // lifespan
    seed = RAND_SEED(seed);
    pet_model[ATTRIB_OFFSET + 11] = lifespan_options[RAND(seed) % 10];
    // affinity
    seed = RAND_SEED(seed);
    pet_model[ATTRIB_OFFSET + 12] = RAND(seed) % 100;
    // morale
    pet_model[ATTRIB_OFFSET + 13] = 0x00;
    // is breedable
    pet_model[BREEDABLE_OFFSET] = 0x00;
    // price
    pet_model[PRICE_OFFSET] = 0x00;

    // build uri
    otxn_accid[0] = count;
    uint8_t hash_out[32];
    util_sha512h(hash_out, 32, SBUF(otxn_accid));

    uint8_t uri_buffer[2 + 219];
    char url[] = "https://api.vpra.app/v1/metadata?account=";
    char address[] = "rUYiTLYpN8M4xLhtRD9HQZFwqZ4WaKJc89";
    char _ns[] = "&namespace=";
    char _key[] = "&key=";

    for (int i = 0; GUARD(41), i < 41; ++i) {
        uri_buffer[i + 2] = url[i];
    }
    for (int i = 0; GUARD(34), i < 34; ++i) {
        uri_buffer[i + 41 + 2] = address[i];
    }
    for (int i = 0; GUARD(11), i < 11; ++i) {
        uri_buffer[i + 75 + 2] = _ns[i];
    }
    
    uint8_t ns[32] = {
        0x3B, 0xE1, 0xD4, 0x22, 0xA4, 0x60, 0x85, 0xE1, 
        0x7F, 0x4A, 0x19, 0xD3, 0xED, 0x70, 0x7C, 0x86, 
        0x69, 0x27, 0xA6, 0xD3, 0x54, 0xA5, 0x62, 0xC4,
        0x5A, 0x78, 0xF1, 0xF0, 0x3D, 0x7C, 0x41, 0x18};
    UINT256_TO_HEXBUF(uri_buffer + 86 + 2, ns);
    for (int i = 0; GUARD(5), i < 5; ++i) {
        uri_buffer[i + 150 + 2] = _key[i];
    }
    UINT256_TO_HEXBUF(uri_buffer + 155 + 2, hash_out);
    UINT256_TO_BUF(DIGEST_OUT, hash_out);
    uint8_t tokenid_buffer[2 + 20 + URI_LEN];
    tokenid_buffer[0] = 0x00U;
    tokenid_buffer[1] = 0x55U;
    ACCOUNT_TO_BUF(tokenid_buffer + 2, hook_acct + 12);
    for (int i = 0; GUARD(URI_LEN), i < URI_LEN; ++i) {
        tokenid_buffer[i + 2 + 20] = uri_buffer[i + 2];
    }
    uint8_t tokenid_out[32];
    util_sha512h(tokenid_out, 32, SBUF(tokenid_buffer));
    
    // token id
    UINT256_TO_BUF(pet_model + TOKENID_OFFSET, tokenid_out);
    // wins
    pet_model[WINS_OFFSET] = 0x00;
    // total races
    pet_model[TOTAL_OFFSET] = 0x00;

    
    etxn_reserve(1);
    uint64_t vl_len = URI_LEN;
    ENCODE_URI(uri_buffer, vl_len);
    PREPARE_REMIT_TXN(hook_acct + 12, otxn_accid + 12, uri_buffer, URI_LEN, txn_currency);

    // TXN: Emit/Send Txn
    uint8_t emithash[32];
    int64_t emit_result = emit(SBUF(emithash), txn, BYTES_LEN + URI_LEN + 2);
    if (emit_result < 0)
    {
        rollback(SBUF("pet_mint.c: Emit Failure."), __LINE__);
    }

    ASSERT(0 < state_set(SBUF(pet_model), hash_out, 32));

    count++;
    state_set(&count, 8, hook_acct + 12, 20);
    ts + DEFAULT_DELAY;
    state_foreign_set(&ts, 8, otxn_accid + 12, 20, SBUF(timer_ns), hook_acct + 12, 20);

    accept(SBUF("pet_mint.c: Finished."), __LINE__);

    _g(1,1);
    return 0;
}