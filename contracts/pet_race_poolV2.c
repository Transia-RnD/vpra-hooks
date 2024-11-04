#include "hookapi.h"

#define DEBUG 1

// -----------------------------------------------------------------------------

#define UINT256_TO_BUF(buf_raw, i)                       \
    {                                                    \
        unsigned char *buf = (unsigned char *)buf_raw;   \
        *(uint64_t *)(buf + 0) = *(uint64_t *)(i + 0);   \
        *(uint64_t *)(buf + 8) = *(uint64_t *)(i + 8);   \
        *(uint64_t *)(buf + 16) = *(uint64_t *)(i + 16); \
        *(uint64_t *)(buf + 24) = *(uint64_t *)(i + 24); \
    }

#define ACCOUNT_TO_BUF(buf_raw, i)                       \
    {                                                    \
        unsigned char *buf = (unsigned char *)buf_raw;   \
        *(uint64_t *)(buf + 0) = *(uint64_t *)(i + 0);   \
        *(uint64_t *)(buf + 8) = *(uint64_t *)(i + 8);   \
        *(uint32_t *)(buf + 16) = *(uint32_t *)(i + 16); \
    }

/**
 *
 * These functions should be moved into the macro.c file
 */

#define float_subtract(x, y)          \
    {                                 \
        float_sum(x, float_negate(y)) \
    }

#define float_delete(key)    \
    {                        \
        state_set(0, 0, key) \
    }

#define BUFFER_SWAP(x, y) \
    {                     \
        uint8_t *z = x;   \
        x = y;            \
        y = z;            \
    }

#define DONE(x) \
    accept(SBUF(x), __LINE__);

// -----------------------------------------------------------------------------

#define INT8_TO_BUF(buf_raw, i)                        \
    {                                                  \
        unsigned char *buf = (unsigned char *)buf_raw; \
        buf[0] = (((uint8_t)i) >> 0) & 0xFFUL;         \
        if (i < 0)                                     \
            buf[0] |= 0x80U;                           \
    }

uint8_t txn[283] =
    {
        /* size,upto */
        /*   3,  0 */ 0x12U, 0x00U, 0x00U,                                    /* tt = Payment */
        /*   5,  3*/ 0x22U, 0x80U, 0x00U, 0x00U, 0x00U,                       /* flags = tfCanonical */
        /*   5,  8 */ 0x24U, 0x00U, 0x00U, 0x00U, 0x00U,                      /* sequence = 0 */
        /*   5, 13 */ 0x99U, 0x99U, 0x99U, 0x99U, 0x99U,                      /* dtag, flipped */
        /*   6, 18 */ 0x20U, 0x1AU, 0x00U, 0x00U, 0x00U, 0x00U,               /* first ledger seq */
        /*   6, 24 */ 0x20U, 0x1BU, 0x00U, 0x00U, 0x00U, 0x00U,               /* last ledger seq */
        /*  49, 30 */ 0x61U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, /* amount field 9 or 49 bytes */
        0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U,
        0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U,
        0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U,
        0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U,
        0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99,
        /*   9, 79 */ 0x68U, 0x40U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,                                                   /* fee      */
        /*  35, 88 */ 0x73U, 0x21U, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* pubkey   */
        /*  22,123 */ 0x81U, 0x14U, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                                        /* src acc  */
        /*  22,145 */ 0x83U, 0x14U, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                                        /* dst acc  */
        /* 116,167 */                                                                                                                  /* emit details */
                                                                                                                                       /*   0,283 */
};

#define FLIP_ENDIAN_64(n) ((uint64_t)(((n & 0xFFULL) << 56ULL) |             \
                                      ((n & 0xFF00ULL) << 40ULL) |           \
                                      ((n & 0xFF0000ULL) << 24ULL) |         \
                                      ((n & 0xFF000000ULL) << 8ULL) |        \
                                      ((n & 0xFF00000000ULL) >> 8ULL) |      \
                                      ((n & 0xFF0000000000ULL) >> 24ULL) |   \
                                      ((n & 0xFF000000000000ULL) >> 40ULL) | \
                                      ((n & 0xFF00000000000000ULL) >> 56ULL)))

#define RACE_MODEL_SIZE 77U
#define STATUS_OFFSET 8U
#define START_OFFSET 49U

#define BET_MODEL_SIZE 50U
#define BET_ODD_OFFSET 9U
#define SLIP_MODEL_SIZE 55U
#define SLIP_POS_OFFSET 8U
#define SLIP_VALUE_OFFSET 9U
#define SLIP_WIN_OFFSET 17U
#define SLIP_TOSEAL_OFFSET 25U
#define SLIP_EXECUTED_OFFSET 33U
uint8_t slip_model[SLIP_MODEL_SIZE] = {};
#define VALUE_OUT (slip_model + SLIP_VALUE_OFFSET)
#define WIN_OUT (slip_model + SLIP_WIN_OFFSET)
#define TO_SEAL_OUT (slip_model + SLIP_TOSEAL_OFFSET)

#define PET_MODEL_SIZE 100U
#define MORALE_OFFSET 35U // field offset from 0
#define WINS_OFFSET 84U // field offset from 0
#define TOTAL_OFFSET 92U // field offset from 0

uint8_t pet_ns[32] = {
    0x3B, 0xE1, 0xD4, 0x22, 0xA4, 0x60, 0x85, 0xE1, 0x7F, 0x4A, 0x19, 0xD3, 
    0xED, 0x70, 0x7C, 0x86, 0x69, 0x27, 0xA6, 0xD3, 0x54, 0xA5, 0x62, 0xC4, 
    0x5A, 0x78, 0xF1, 0xF0, 0x3D, 0x7C, 0x41, 0x18
};

uint8_t races_ns[32] = {
    0xB1, 0x4F, 0x0B, 0xA3, 0x1B, 0x19, 0xC8, 0x8A, 0x24, 0x93, 0xFC, 0x79, 
    0x00, 0xFE, 0xA5, 0x05, 0x07, 0x02, 0x0E, 0x05, 0xC5, 0x61, 0x2A, 0xAB, 
    0x27, 0x0E, 0x42, 0x2F, 0x0C, 0xDF, 0x8D, 0xCE
};

uint8_t curr_buffer[20] = {
    0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
    0x00U, 0x00U, 0x50U, 0x58U, 0x50U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U
};

uint8_t admin_buffer[20] = {
    0xCDU, 0x50U, 0x93U, 0x6EU, 0x7DU, 0x42U, 0x72U, 0xADU, 0x5FU, 0xB2U, 
    0x73U, 0x21U, 0xE7U, 0x1EU, 0xE5U, 0x19U, 0x57U, 0x0FU, 0xC2U, 0xE7U
};

// LOCAL
// uint8_t admin_buffer[20] = {
//     0x12U, 0x10U, 0x24U, 0x04U, 0x90U, 0x1EU, 0x28U, 0x8DU, 0x25U, 0xADU, 
//     0x11U, 0xD3U, 0xEAU, 0x55U, 0xA0U, 0x5BU, 0xBAU, 0x07U, 0x7FU, 0x4DU
// };

#define ttSET_HOOK 22U

int64_t hook(uint32_t reserved)
{
    _g(1, 1);
    TRACESTR("pet_race_pool.c: Start.");

    // ACCOUNT: Hook Account
    uint8_t hook_accid[32];
    hook_account(hook_accid + 12, 20);

    // ACCOUNT: Origin Tx Account
    uint8_t otxn_accid[32];
    otxn_field(otxn_accid + 12, 20, sfAccount);
    TRACEHEX(otxn_accid);

    // END RACE
    uint8_t end_race[97];
    uint8_t er_key[2] = {'E', 'R'};
    if (otxn_param(SBUF(end_race), SBUF(er_key)) > 0 && otxn_type() == ttSET_HOOK && BUFFER_EQUAL_20(otxn_accid + 12, admin_buffer))
    {
        etxn_reserve(4);

        uint8_t end_race_bets[1024];
        uint8_t erb_key[3] = {'E', 'R', 'B'};
        otxn_param(SBUF(end_race_bets), SBUF(erb_key));

        uint8_t race_hash[32];
        uint8_t rh_key[2] = {'R', 'H'};
        otxn_param(SBUF(race_hash), SBUF(rh_key));

        uint8_t rp_ns[32];
        uint8_t rpn_key[3] = {'R', 'P', 'N'};
        otxn_param(SBUF(rp_ns), SBUF(rpn_key));

        int64_t num_bets = end_race_bets[0];
        TRACEVAR(num_bets);
        
        int64_t buffer_offset = 1;
        for (int bi = 0; GUARD(5), bi < num_bets; ++bi)
        {
            uint8_t bet_hash[32];
            UINT256_TO_BUF(bet_hash, end_race_bets + buffer_offset);
            buffer_offset += 32;

            uint8_t bet_position = end_race_bets[buffer_offset];
            buffer_offset += 1;

            int64_t num_slips = end_race_bets[buffer_offset];
            TRACEVAR(num_slips);

            buffer_offset += 1;

            for (int si = 0; GUARD(10), si < num_slips; ++si)
            {
                uint8_t slip_hash[32];
                UINT256_TO_BUF(slip_hash, end_race_bets + buffer_offset);
                buffer_offset += 32;

                uint8_t slip_model[SLIP_MODEL_SIZE];
                if (state_foreign(SBUF(slip_model), SBUF(slip_hash), SBUF(bet_hash), hook_accid + 12, 20) == DOESNT_EXIST)
                {
                    TRACESTR("pet_race_pool.c: Invalid Slip Model.");
                    // pass
                }
                TRACEHEX(slip_model);
#define UPDATE_EXP_OUT (slip_model + SLIP_EXECUTED_OFFSET)

                int64_t slip_pos = (int64_t)slip_model[SLIP_POS_OFFSET];
                TRACEVAR(slip_pos);
                TRACEVAR(bet_position);
                if (bet_position != slip_pos)
                {
                    INT8_TO_BUF(UPDATE_EXP_OUT, 1);
                    state_foreign_set(SBUF(slip_model), SBUF(slip_hash), SBUF(bet_hash), hook_accid + 12, 20);
                    TRACESTR("pet_race_pool.c: Skipping Slip - wrong win position");
                    continue;
                }

                int64_t executed = (int64_t)slip_model[SLIP_EXECUTED_OFFSET];
                if (executed == 1)
                {
                    TRACESTR("pet_race_pool.c: Skipping Slip - executed win position");
                    continue;
                }

                int64_t to_seal_xfl = FLIP_ENDIAN_64(UINT64_FROM_BUF(slip_model + SLIP_TOSEAL_OFFSET));
                if (to_seal_xfl != 0)
                {
                    INT8_TO_BUF(UPDATE_EXP_OUT, 1);
                    state_foreign_set(SBUF(slip_model), SBUF(slip_hash), SBUF(bet_hash), hook_accid + 12, 20);
                    TRACESTR("pet_race_pool.c: Skipping Slip - not sealed.");
                    continue;
                }
                int64_t value_xfl = FLIP_ENDIAN_64(UINT64_FROM_BUF(slip_model + SLIP_VALUE_OFFSET));
                int64_t win_xfl = FLIP_ENDIAN_64(UINT64_FROM_BUF(slip_model + SLIP_WIN_OFFSET));
                int64_t total_xfl = float_sum(value_xfl, win_xfl);
                TRACEVAR(total_xfl)
                TRACEVAR(to_seal_xfl)
                TRACEVAR(value_xfl)
                TRACEVAR(win_xfl)

                TRACEHEX(slip_model);

#define OTXN_ACC (txn + 125U)
#define DEST_ACC (txn + 147U)
#define FLS_OUT (txn + 20U)
#define LLS_OUT (txn + 26U)
#define DTAG_OUT (txn + 14U)
#define AMOUNT_OUT (txn + 30U)
#define EMIT_OUT (txn + 167U)
#define FEE_OUT (txn + 80U)

                // TXN: PREPARE: Init
                // etxn_reserve(1);

                // TXN PREPARE: Account
                ACCOUNT_TO_BUF(OTXN_ACC, hook_accid + 12);

                // TXN PREPARE: Destination
                ACCOUNT_TO_BUF(DEST_ACC, slip_hash + 12);

                // TXN PREPARE: FirstLedgerSequence
                uint32_t fls = (uint32_t)ledger_seq() + 1;
                *((uint32_t *)(FLS_OUT)) = FLIP_ENDIAN(fls);

                // TXN PREPARE: LastLedgerSequense
                uint32_t lls = fls + 4;
                *((uint32_t *)(LLS_OUT)) = FLIP_ENDIAN(lls);

                // TXN PREPARE: Amount
                float_sto(AMOUNT_OUT, 49, curr_buffer, 20, hook_accid + 12, 20, total_xfl, sfAmount);

                // TXN PREPARE: Dest Tag <- Source Tag
                if (otxn_field(DTAG_OUT, 4, sfSourceTag) == 4)
                    *(DTAG_OUT - 1) = 0x2EU;

                // TXN PREPARE: Emit Metadata
                etxn_details(EMIT_OUT, 116U);

                // TXN PREPARE: Fee
                {
                    int64_t fee = etxn_fee_base(SBUF(txn));
                    if (DEBUG)
                        TRACEVAR(fee);
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
                TRACEHEX(txn);

                if (DEBUG)
                    trace(SBUF("txnraw"), SBUF(txn), 1);

                // TXN: Emit/Send Txn
                uint8_t emithash[32];
                int64_t emit_result = emit(SBUF(emithash), SBUF(txn));
                if (emit_result < 0)
                {
                    rollback(SBUF("pet_race_pool.c: Not Emitted."), __LINE__);
                }
                // STATE: SET
                INT8_TO_BUF(UPDATE_EXP_OUT, 1);
                state_foreign_set(SBUF(slip_model), SBUF(slip_hash), SBUF(bet_hash), hook_accid + 12, 20);
                TRACESTR(SBUF("pet_race_pool.c: Emitted."));
                continue;
                TRACESTR(SBUF("pet_race_pool.c: Not Emitted."));
            }
        }
        
        TRACEHEX(end_race);
        int64_t num_winners = (int64_t)end_race[0];
        uint64_t prizes[5];
        prizes[0] = 6077852297695428608; // 0.7 %
        prizes[1] = 6072852297695428608; // 0.2 %
        prizes[2] = 6071852297695428608; // 0.1 %
        prizes[3] = 0; // 0 %
        prizes[4] = 0; // 0 %

        uint8_t prize_pool[8];
        state_foreign(SBUF(prize_pool), SBUF(rp_ns), SBUF(race_hash), hook_accid + 12, 20);
        TRACEHEX(prize_pool);
        int64_t prize_pool_xfl = *((int64_t*)prize_pool);
        TRACEVAR(prize_pool_xfl);
        TRACEVAR(num_winners);
        for (int wi = 0; GUARD(5), wi < num_winners; ++wi)
        {
            int64_t payout_xfl = float_multiply(prize_pool_xfl, prizes[wi]);
            TRACEVAR(payout_xfl);

            uint8_t ptid_buff[34];
            util_keylet(SBUF(ptid_buff), KEYLET_UNCHECKED, end_race + 1U + (wi * 32), 32, 0, 0, 0, 0);
            if (slot_set(SBUF(ptid_buff), 1) != 1)
                rollback(SBUF("pet_race_pool.c: Could not load keylet `Pet`"), __LINE__);
            if (slot_subfield(1, sfOwner, 2) != 2)
                rollback(SBUF("pet_race_pool.c: Could not load `Pet` sfOwner"), __LINE__);
            if (slot_subfield(1, sfDigest, 3) != 3)
                rollback(SBUF("pet_race_pool.c: Could not load `Pet` sfDigest"), __LINE__);

            uint8_t pet_owner[20];
            slot(SBUF(pet_owner), 2);

            uint8_t pet_digest[32];
            slot(SBUF(pet_digest), 3);

            uint8_t pet_model[PET_MODEL_SIZE];
            state_foreign(SBUF(pet_model), pet_digest, 32, SBUF(pet_ns), hook_accid + 12, 20);
            TRACEHEX(pet_model);
            
            // update morale
#define MORALE_OUT (pet_model + MORALE_OFFSET)
            if (wi == 0)
            {
                // +1
                UINT64_TO_BUF(MORALE_OUT, UINT64_FROM_BUF(pet_model + WINS_OFFSET) + 1);
            }
            else
            {
                // reset morale
                UINT64_TO_BUF(MORALE_OUT, 0);
            }
            // update wins
#define WINS_OUT (pet_model + WINS_OFFSET)
            UINT64_TO_BUF(WINS_OUT, UINT64_FROM_BUF(pet_model + WINS_OFFSET) + 1);
            // update total
#define TOTAL_OUT (pet_model + TOTAL_OFFSET)
            INT64_TO_BUF(TOTAL_OUT, float_sum(payout_xfl, UINT64_FROM_BUF(pet_model + TOTAL_OFFSET)));

            // TXN PREPARE: Account
            ACCOUNT_TO_BUF(OTXN_ACC, hook_accid + 12);

            // TXN PREPARE: Destination
            ACCOUNT_TO_BUF(DEST_ACC, pet_owner);

            // TXN PREPARE: FirstLedgerSequence
            uint32_t fls = (uint32_t)ledger_seq() + 1;
            *((uint32_t *)(FLS_OUT)) = FLIP_ENDIAN(fls);

            // TXN PREPARE: LastLedgerSequense
            uint32_t lls = fls + 4;
            *((uint32_t *)(LLS_OUT)) = FLIP_ENDIAN(lls);

            // TXN PREPARE: Amount
            float_sto(AMOUNT_OUT, 49, curr_buffer, 20, hook_accid + 12, 20, payout_xfl, sfAmount);

            // TXN PREPARE: Dest Tag <- Source Tag
            if (otxn_field(DTAG_OUT, 4, sfSourceTag) == 4)
                *(DTAG_OUT - 1) = 0x2EU;

            // TXN PREPARE: Emit Metadata
            etxn_details(EMIT_OUT, 116U);

            // TXN PREPARE: Fee
            {
                int64_t fee = etxn_fee_base(SBUF(txn));
                if (DEBUG)
                    TRACEVAR(fee);
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
            TRACEHEX(txn);

            if (DEBUG)
                trace(SBUF("txnraw"), SBUF(txn), 1);

            // TXN: Emit/Send Txn
            uint8_t emithash[32];
            int64_t emit_result = emit(SBUF(emithash), SBUF(txn));
            if (emit_result < 0)
            {
                rollback(SBUF("pet_race_pool.c: Not Emitted."), __LINE__);
            }
            // STATE: SET - Update Pet (Remove from race)
            state_foreign_set(SBUF(pet_model), pet_digest, 32, SBUF(pet_ns), hook_accid + 12, 20);
            TRACESTR(SBUF("pet_race_pool.c: Emitted."));
            continue;
        }

        uint8_t race_model[RACE_MODEL_SIZE];
        state_foreign(SBUF(race_model), SBUF(race_hash), SBUF(races_ns), hook_accid + 12, 20);
        INT8_TO_BUF(race_model + STATUS_OFFSET, 0);
        UINT32_TO_BUF(race_model + START_OFFSET, 0);
        state_foreign_set(SBUF(race_model), SBUF(race_hash), SBUF(races_ns), hook_accid + 12, 20);
        DONE("pet_race_pool.c: Race Ended.");
    }

    if (otxn_type() == ttSET_HOOK)
        DONE("pet_race_pool.c: Passing SetHook.");

    // CREATE HORSE BET
    uint8_t rb_ns[32];
    uint8_t rbn_key[3] = {'R', 'B', 'N'};
    if (otxn_param(SBUF(rb_ns), SBUF(rbn_key)) != 32)
    {
        rollback(SBUF("pet_race_pool.c: Invalid Txn Parameter `RBN`"), __LINE__);
    }
    TRACEHEX(rb_ns);

    uint8_t rb_hash[32];
    uint8_t rb_key[3] = {'R', 'B', 'H'};
    if (otxn_param(SBUF(rb_hash), SBUF(rb_key)) != 32)
    {
        rollback(SBUF("pet_race_pool.c: Invalid Txn Parameter `RBH`"), __LINE__);
    }

    // VALIDATE: Pet Track & Race

    uint8_t bet_state[BET_MODEL_SIZE];
    uint8_t bm_key[2] = {'B', 'M'};
    if (otxn_param(SBUF(bet_state), SBUF(bm_key)) == BET_MODEL_SIZE && otxn_type() == ttINVOKE && !BUFFER_EQUAL_20(hook_accid + 12, otxn_accid + 12))
    {
        uint8_t bet_dump[BET_MODEL_SIZE];
        if (state(SBUF(bet_dump), SBUF(rb_hash)) == BET_MODEL_SIZE)
        {
            DONE("pet_race_pool.c: Cannot update bet after creation.");
        }
        state_foreign_set(SBUF(bet_state), hook_accid + 12, 20, SBUF(rb_ns), hook_accid + 12, 20);
        DONE("pet_race_pool.c: Bet Created.");
    }

    uint8_t bet_model[BET_MODEL_SIZE];
    if (state_foreign(SBUF(bet_model), hook_accid + 12, 20, SBUF(rb_ns), hook_accid + 12, 20) == DOESNT_EXIST)
    {
        DONE("pet_race_pool.c: Invalid Bet Model State.");
    }

    TRACEHEX(bet_model);

    // CREATE HORSE SLIP
    uint8_t dump[SLIP_MODEL_SIZE];
    if (state_foreign(SBUF(dump), otxn_accid + 12, 20, SBUF(rb_hash), hook_accid + 12, 20) == SLIP_MODEL_SIZE)
    {
        DONE("pet_race_pool.c: Cannot update slip after submission.");
    }

    uint8_t rbs_hash[32];
    uint8_t rbs_key[4] = {'R', 'B', 'S', 'H'};
    if (otxn_param(SBUF(rbs_hash), SBUF(rbs_key)) != 32)
    {
        rollback(SBUF("pet_race_pool.c: Invalid Txn Parameter `RBSH`"), __LINE__);
    }

    // uint8_t slip_model[SLIP_MODEL_SIZE];
    uint8_t sm_key[2] = {'S', 'M'};
    if (otxn_param(SBUF(slip_model), SBUF(sm_key)) != SLIP_MODEL_SIZE)
    {
        rollback(SBUF("pet_race_pool.c: Invalid Txn Parameter `SM`"), __LINE__);
    }

    int64_t amount_xfl = -1;
    otxn_slot(1);
    if (slot_subfield(1, sfAmount, 1) == 1)
    {
        amount_xfl = slot_float(1);
    }

    int64_t position = (int64_t)slip_model[SLIP_POS_OFFSET];
    TRACEVAR(position);

    int64_t win_xfl = 0;
    int64_t newValue = 0;

    // ODD
    int64_t odd_xfl = FLIP_ENDIAN_64(UINT64_FROM_BUF(bet_model + BET_ODD_OFFSET));
    TRACEVAR(amount_xfl);
    TRACEVAR(odd_xfl);

    if (position == 0)
    {
        rollback(SBUF("pet_race_pool.c: Invalid Slip Parameter `position`"), __LINE__);
    }

    // 1 for yes
    if (position == 1)
    {
        win_xfl = float_multiply(amount_xfl, odd_xfl);
    }
    // 2 for no
    if (position == 2)
    {
        win_xfl = float_divide(amount_xfl, odd_xfl);
    }
    TRACEVAR(win_xfl);
    INT64_TO_BUF(WIN_OUT, FLIP_ENDIAN_64(win_xfl));

    // UPDATE PRIZE POOL
    int64_t pp_xfl = float_sum(amount_xfl, win_xfl);
    TRACEVAR(pp_xfl);

    TRACEHEX(bet_model);
    TRACEHEX(slip_model);

    int64_t nv_xfl = amount_xfl;
    int8_t is_sealed = 0;

    uint8_t cs_hash[32];
    uint8_t cs_key[3] = {'C', 'S', 'H'};
    if (otxn_param(SBUF(cs_hash), SBUF(cs_key)) == 32)
    {
        uint8_t c_slip_model[SLIP_MODEL_SIZE];
        if (state_foreign(SBUF(c_slip_model), SBUF(cs_hash), SBUF(rb_hash), hook_accid + 12, 20) == DOESNT_EXIST)
        {
            DONE("pet_race_pool.c: Invalid Close Slip `Model`.");
        }

        TRACEHEX(cs_hash);
        TRACEHEX(c_slip_model);

        int64_t slip_pos = (int64_t)c_slip_model[0];

        TRACEVAR(position);
        TRACEVAR(slip_pos);
        if (position == slip_pos)
        {
            DONE("pet_race_pool.c: Invalid Close Slip `Position`.");
        }

        uint8_t to_seal[8];
        to_seal[0] = c_slip_model[17];
        to_seal[1] = c_slip_model[18];
        to_seal[2] = c_slip_model[19];
        to_seal[3] = c_slip_model[20];
        to_seal[4] = c_slip_model[21];
        to_seal[5] = c_slip_model[22];
        to_seal[6] = c_slip_model[23];
        to_seal[7] = c_slip_model[24];

        TRACEHEX(to_seal);

        int64_t cs_seal_xfl = FLIP_ENDIAN_64(UINT64_FROM_BUF(to_seal));
        TRACEVAR(cs_seal_xfl);
        TRACEVAR(nv_xfl);
        if (cs_seal_xfl > nv_xfl)
        {
            DONE("pet_race_pool.c: Skip ToSeal > New Value.");
        }

        nv_xfl = float_sum(nv_xfl, float_negate(cs_seal_xfl));
        is_sealed = 1;
        TRACEVAR(nv_xfl);
        TRACESTR("pet_race_pool.c: Sealed.");

#define UPDATE_CS_SEAL_OUT (c_slip_model + SLIP_TOSEAL_OFFSET);
        INT64_TO_BUF(UPDATE_CS_SEAL_OUT, 0);

        // update close slip
        state_foreign_set(SBUF(c_slip_model), SBUF(cs_hash), SBUF(rb_hash), hook_accid + 12, 20);
        TRACESTR("pet_race_pool.c: Add slip to sealed list.");
    }

    if (is_sealed == 0)
    {
        nv_xfl = win_xfl;
        TRACESTR("pet_race_pool.c: Slip Already Sealed.");
    }

    // update slip
    INT64_TO_BUF(TO_SEAL_OUT, FLIP_ENDIAN_64(nv_xfl));
    state_foreign_set(SBUF(slip_model), otxn_accid + 12, 20, SBUF(rb_hash), hook_accid + 12, 20);

    accept(SBUF("pet_race_pool.c: Slip Created."), __LINE__);
    // unreachable
    return 0;
}