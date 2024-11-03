import {
  Wallet,
  Invoke,
  Payment,
  SetHookFlags,
  TransactionMetadata,
  AccountObjectsResponse,
} from '@transia/xrpl'
// xrpl-helpers
import {
  serverUrl,
  XrplIntegrationTestContext,
  setupClient,
  teardownClient,
  trust,
  IC,
  close,
} from '@transia/hooks-toolkit/dist/npm/src/libs/xrpl-helpers'
// src
import {
  Xrpld,
  SetHookParams,
  createHookPayload,
  setHooksV3,
  SetHookPayload,
  iHookParamEntry,
  iHookParamName,
  iHookParamValue,
  ExecutionUtility,
  hexNamespace,
  clearHookStateV3,
  iHook,
  padHexString,
  floatToLEXfl,
} from '@transia/hooks-toolkit'
import { REBetModel, REHashModel, REBetsModel, REModel } from './models/REModel'
import { RaceBetModel } from './models/RaceBetModel'
import { RaceSlipModel } from './models/RaceSlipModel'
import { xrpAddressToHex } from '@transia/hooks-toolkit/dist/npm/src/libs/binary-models'
import { IssuedCurrencyAmount } from '@transia/xrpl/dist/npm/models/common'

// Pets/#
// Tracks/#
// Tracks/#/Pools/#
// Tracks/#/Pools/#/Bets/#
// Tracks/#/Pools/#/Bets/#/Slips/#

export async function createPetBet(
  testContext: XrplIntegrationTestContext,
  fromWallet: Wallet,
  destination: string,
  raceId: number,
  odd: number,
  petHash: string
) {
  const testBet = new RaceBetModel(
    BigInt(1), // id
    0, // 0 (Deployed), 1 (Ended) or 2 (Settled)
    odd, // Odds Ex: 2:1
    petHash, // 32 char hash
    0 // winning position (for closing bets)
  )
  console.log(testBet.encode())
  console.log(testBet.encode().length / 2)

  const betId = 1

  const ns = hexNamespace(`/races/${raceId}/bets`)
  const hash = hexNamespace(`/races/${raceId}/bets/${betId}`)

  console.log(ns)
  console.log(hash)

  const txParam1 = new iHookParamEntry(
    new iHookParamName('HPA'),
    new iHookParamValue('0A01000000000100000000', true)
  )
  const txParam2 = new iHookParamEntry(
    new iHookParamName('RBN'),
    new iHookParamValue(ns, true)
  )
  const txParam3 = new iHookParamEntry(
    new iHookParamName('RBH'),
    new iHookParamValue(hash, true)
  )
  const txParam4 = new iHookParamEntry(
    new iHookParamName('BM'),
    new iHookParamValue(testBet.encode(), true)
  )
  const builtTx1: Invoke = {
    TransactionType: 'Invoke',
    Account: fromWallet.classicAddress,
    Destination: destination,
    HookParameters: [
      txParam1.toXrpl(),
      txParam2.toXrpl(),
      txParam3.toXrpl(),
      txParam4.toXrpl(),
    ],
  }
  const result1 = await Xrpld.submit(testContext.client, {
    wallet: fromWallet,
    tx: builtTx1,
  })
  const hookExecutions1 = await ExecutionUtility.getHookExecutionsFromMeta(
    testContext.client,
    result1.meta as TransactionMetadata
  )
  expect(hookExecutions1.executions[1].HookReturnString).toEqual(
    'pet_race_pool.c: Bet Created.'
  )
}

export async function createPetSlip(
  testContext: XrplIntegrationTestContext,
  fromWallet: Wallet,
  destination: string,
  value: number,
  slipId: number,
  position: number
) {
  const testSlip = new RaceSlipModel(
    BigInt(slipId),
    position,
    value,
    0,
    0,
    0,
    0,
    fromWallet.classicAddress
  )

  const raceId = 1
  const betId = 1

  const ns = hexNamespace(`/races/${raceId}/bets`)
  const betHash = hexNamespace(`/races/${raceId}/bets/${betId}`)
  const hash = hexNamespace(`/races/${raceId}/bets/${betId}/slips/${slipId}`)

  const txParam1 = new iHookParamEntry(
    new iHookParamName('HPA'),
    new iHookParamValue('0A01000000000100000000', true)
  )
  const txParam2 = new iHookParamEntry(
    new iHookParamName('RBN'),
    new iHookParamValue(ns, true)
  )
  const txParam3 = new iHookParamEntry(
    new iHookParamName('RBH'),
    new iHookParamValue(betHash, true)
  )
  const txParam4 = new iHookParamEntry(
    new iHookParamName('RBSH'),
    new iHookParamValue(hash, true)
  )
  const txParam5 = new iHookParamEntry(
    new iHookParamName('SM'),
    new iHookParamValue(testSlip.encode(), true)
  )
  const amount: IssuedCurrencyAmount = {
    issuer: testContext.hook1.classicAddress,
    currency: 'PXP',
    value: String(value),
  }
  const builtTx1: Payment = {
    TransactionType: 'Payment',
    Account: fromWallet.classicAddress,
    Destination: destination,
    Amount: amount,
    HookParameters: [
      txParam1.toXrpl(),
      txParam2.toXrpl(),
      txParam3.toXrpl(),
      txParam4.toXrpl(),
      txParam5.toXrpl(),
    ],
  }
  const result1 = await Xrpld.submit(testContext.client, {
    wallet: fromWallet,
    tx: builtTx1,
  })
  const hookExecutions1 = await ExecutionUtility.getHookExecutionsFromMeta(
    testContext.client,
    result1.meta as TransactionMetadata
  )
  expect(hookExecutions1.executions[1].HookReturnString).toEqual(
    'pet_race_pool.c: Slip Created.'
  )
}

export async function closePetSlip(
  testContext: XrplIntegrationTestContext,
  fromWallet: Wallet,
  destination: string,
  value: number,
  slipId: number,
  position: number,
  closeKey: string
) {
  const testSlip = new RaceSlipModel(
    BigInt(slipId),
    position,
    value,
    0,
    0,
    0,
    0,
    fromWallet.classicAddress
  )
  console.log(testSlip.encode())
  console.log(testSlip.encode().length / 2)

  const raceId = 1
  const betId = 1

  const ns = hexNamespace(`/races/${raceId}/bets`)
  const betHash = hexNamespace(`/races/${raceId}/bets/${betId}`)
  const hash = hexNamespace(`/races/${raceId}/bets/${betId}/slips/${slipId}`)

  const txParam1 = new iHookParamEntry(
    new iHookParamName('HPA'),
    new iHookParamValue('0A01000000000100000000', true)
  )
  const txParam2 = new iHookParamEntry(
    new iHookParamName('RBN'),
    new iHookParamValue(ns, true)
  )
  const txParam3 = new iHookParamEntry(
    new iHookParamName('RBH'),
    new iHookParamValue(betHash, true)
  )
  const txParam4 = new iHookParamEntry(
    new iHookParamName('RBSH'),
    new iHookParamValue(hash, true)
  )
  const txParam5 = new iHookParamEntry(
    new iHookParamName('SM'),
    new iHookParamValue(testSlip.encode(), true)
  )
  const txParam6 = new iHookParamEntry(
    new iHookParamName('CSH'),
    new iHookParamValue(closeKey, true)
  )
  const amount: IssuedCurrencyAmount = {
    issuer: testContext.hook1.classicAddress,
    currency: 'PXP',
    value: String(value),
  }
  const builtTx1: Payment = {
    TransactionType: 'Payment',
    Account: fromWallet.classicAddress,
    Destination: destination,
    Amount: amount,
    HookParameters: [
      txParam1.toXrpl(),
      txParam2.toXrpl(),
      txParam3.toXrpl(),
      txParam4.toXrpl(),
      txParam5.toXrpl(),
      txParam6.toXrpl(),
    ],
  }
  const result1 = await Xrpld.submit(testContext.client, {
    wallet: fromWallet,
    tx: builtTx1,
  })
  const hookExecutions1 = await ExecutionUtility.getHookExecutionsFromMeta(
    testContext.client,
    result1.meta as TransactionMetadata
  )
  expect(hookExecutions1.executions[1].HookReturnString).toEqual(
    'pet_race_pool.c: Slip Created.'
  )
}

export async function endRace(testContext: XrplIntegrationTestContext) {
  const hookWallet = testContext.hook1
  const triggerWallet = testContext.hook2

  const raceId = 1
  const betId = 1
  const betHash = hexNamespace(`/races/${raceId}/bets/${betId}`)

  const bet1slip1: REHashModel = new REHashModel(
    padHexString(xrpAddressToHex(testContext.alice.classicAddress))
  )
  const bet1slip2: REHashModel = new REHashModel(
    padHexString(xrpAddressToHex(testContext.carol.classicAddress))
  )
  const bet1: REBetModel = new REBetModel(betHash, 1, [bet1slip1, bet1slip2])
  const endRaceBets: REBetsModel = new REBetsModel([bet1])
  console.log(endRaceBets)
  console.log(endRaceBets.encode())

  const CAROL_ID: string = (
    (await testContext.client.request({
      command: 'account_objects',
      account: testContext.carol.classicAddress,
      type: 'uri_token',
    })) as AccountObjectsResponse
  ).result.account_objects[0].index
  const pet1: REHashModel = new REHashModel(CAROL_ID)
  const DAVE_ID: string = (
    (await testContext.client.request({
      command: 'account_objects',
      account: testContext.dave.classicAddress,
      type: 'uri_token',
    })) as AccountObjectsResponse
  ).result.account_objects[0].index
  const pet2: REHashModel = new REHashModel(DAVE_ID)
  // const BOB_ID: string = (
  //   (await testContext.client.request({
  //     command: 'account_objects',
  //     account: testContext.bob.classicAddress,
  //     type: 'uri_token',
  //   })) as AccountObjectsResponse
  // ).result.account_objects[0].index
  // const pet3: REHashModel = new REHashModel(BOB_ID)
  // const endRace: REModel = new REModel([pet1, pet2, pet3])
  const endRace: REModel = new REModel([pet1, pet2])
  console.log(endRace.encode().length / 2)

  const hash = hexNamespace(`/races/${raceId}`)
  const rpns = hexNamespace(`/races/${raceId}/pool`)

  const txParam1 = new iHookParamEntry(
    new iHookParamName('HPA'),
    new iHookParamValue('0A01000000000100000000', true)
  )
  const txParam2 = new iHookParamEntry(
    new iHookParamName('RPN'),
    new iHookParamValue(rpns, true)
  )
  const txParam3 = new iHookParamEntry(
    new iHookParamName('RH'),
    new iHookParamValue(hash, true)
  )
  // const txParam4 = new iHookParamEntry(
  //   new iHookParamName('ERB'),
  //   new iHookParamValue(endRaceBets.encode(), true)
  // )
  const txParam5 = new iHookParamEntry(
    new iHookParamName('ER'),
    new iHookParamValue(endRace.encode(), true)
  )
  const builtTx1: Invoke = {
    TransactionType: 'Invoke',
    Account: triggerWallet.classicAddress,
    Destination: hookWallet.classicAddress,
    HookParameters: [
      txParam1.toXrpl(),
      txParam2.toXrpl(),
      txParam3.toXrpl(),
      // txParam4.toXrpl(),
      txParam5.toXrpl(),
    ],
  }
  const result1 = await Xrpld.submit(testContext.client, {
    wallet: triggerWallet,
    tx: builtTx1,
  })
  const hookExecutions1 = await ExecutionUtility.getHookExecutionsFromMeta(
    testContext.client,
    result1.meta as TransactionMetadata
  )
  expect(hookExecutions1.executions[1].HookReturnString).toEqual(
    'pet_race_pool.c: Race Ended.'
  )
  await close(testContext.client)
}

describe('Pets - Success Group', () => {
  let testContext: XrplIntegrationTestContext

  beforeAll(async () => {
    testContext = await setupClient(serverUrl)
    const hookWallet = testContext.hook1

    const gwWallet = testContext.gw
    const USD = IC.gw('USD', gwWallet.classicAddress)
    await trust(testContext.client, USD.set(100000), ...[hookWallet])

    const hook1 = createHookPayload({
      version: 0,
      createFile: 'router_base',
      namespace: 'pets',
      flags: SetHookFlags.hsfOverride,
      hookOnArray: ['Payment', 'Invoke'],
    } as SetHookPayload)
    const hook2Param1 = new iHookParamEntry(
      new iHookParamName('MP'),
      new iHookParamValue(floatToLEXfl('10'), true)
    )
    const hook2 = createHookPayload({
      version: 0,
      createFile: 'pet_mintV2',
      namespace: 'pets',
      flags: SetHookFlags.hsfOverride,
      hookOnArray: ['Payment'],
      hookParams: [hook2Param1.toXrpl()],
    } as SetHookPayload)
    const hook3 = createHookPayload({
      version: 0,
      createFile: 'pet_updateV2',
      namespace: 'pets',
      flags: SetHookFlags.hsfOverride,
      hookOnArray: ['Invoke'],
    } as SetHookPayload)
    const hook4 = createHookPayload({
      version: 0,
      createFile: 'pet_breedV2',
      namespace: 'pets',
      flags: SetHookFlags.hsfOverride,
      hookOnArray: ['Payment'],
    } as SetHookPayload)
    const hook5 = createHookPayload({
      version: 0,
      createFile: 'pet_raceV2',
      namespace: '/races',
      flags: SetHookFlags.hsfOverride,
      hookOnArray: ['Payment', 'Invoke'],
    } as SetHookPayload)
    const hook6 = createHookPayload({
      version: 0,
      createFile: 'pet_race_poolV2',
      namespace: '/races',
      flags: SetHookFlags.hsfOverride,
      hookOnArray: ['Payment', 'Invoke'],
    } as SetHookPayload)
    await setHooksV3({
      client: testContext.client,
      seed: testContext.hook1.seed,
      hooks: [
        { Hook: hook1 },
        { Hook: hook2 },
        { Hook: hook3 },
        { Hook: hook4 },
        { Hook: hook5 },
        { Hook: hook6 },
      ],
    } as SetHookParams)
    console.log(
      JSON.stringify([
        { Hook: hook1 },
        { Hook: hook2 },
        { Hook: hook3 },
        { Hook: hook4 },
        { Hook: hook5 },
        { Hook: hook6 },
      ])
    )
    const ns = hexNamespace(`/races/${1}/bets/${1}`)
    const clearHook = {
      Flags: SetHookFlags.hsfNSDelete,
      HookNamespace: ns,
    } as iHook
    await clearHookStateV3({
      client: testContext.client,
      seed: testContext.hook1.seed,
      hooks: [{ Hook: clearHook }],
    } as SetHookParams)
  })
  afterAll(async () => teardownClient(testContext))

  it('create pet bet', async () => {
    const CAROL_ID: string = (
      (await testContext.client.request({
        command: 'account_objects',
        account: testContext.carol.classicAddress,
        type: 'uri_token',
      })) as AccountObjectsResponse
    ).result.account_objects[0].index
    await createPetBet(
      testContext,
      testContext.alice,
      testContext.hook1.classicAddress,
      1,
      2,
      CAROL_ID
    )
  })
  it('create pet slip', async () => {
    await createPetSlip(
      testContext,
      testContext.alice,
      testContext.hook1.classicAddress,
      10,
      1,
      1
    )
    await closePetSlip(
      testContext,
      testContext.bob,
      testContext.hook1.classicAddress,
      20,
      2,
      2,
      padHexString(xrpAddressToHex(testContext.alice.classicAddress))
    )
    await createPetSlip(
      testContext,
      testContext.carol,
      testContext.hook1.classicAddress,
      10,
      1,
      1
    )
  })
  it('end race', async () => {
    console.log('END RACE')
    await endRace(testContext)
  })
})
