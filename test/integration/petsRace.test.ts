import {
  Wallet,
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
  // generateHash,
  hexNamespace,
  clearHookStateV3,
  iHook,
} from '@transia/hooks-toolkit'
import { RaceModel } from './models/RaceModel'
import { IssuedCurrencyAmount } from '@transia/xrpl/dist/npm/models/common'

export async function addPetToRace(
  testContext: XrplIntegrationTestContext,
  fromWallet: Wallet,
  destination: string,
  raceId: number,
  petHash: string
) {
  // Collection Name Space
  const cns = hexNamespace(`/races`)

  // Document Name Space
  const dns = hexNamespace(`/races/${raceId}`)

  // Document Pool Name Space
  const dpns = hexNamespace(`/races/${raceId}/pool`)

  const txParam1 = new iHookParamEntry(
    new iHookParamName('HPA'),
    new iHookParamValue('0A01000000010000000000', true)
  )
  const txParam2 = new iHookParamEntry(
    new iHookParamName('RCN'),
    new iHookParamValue(cns, true)
  )
  const txParam3 = new iHookParamEntry(
    new iHookParamName('RH'),
    new iHookParamValue(dns, true)
  )
  const txParam4 = new iHookParamEntry(
    new iHookParamName('RPN'),
    new iHookParamValue(dpns, true)
  )
  const txParam5 = new iHookParamEntry(
    new iHookParamName('PH'),
    new iHookParamValue(petHash, true)
  )
  const amount: IssuedCurrencyAmount = {
    issuer: testContext.hook1.classicAddress,
    currency: 'PXP',
    value: '100',
  }
  const builtTx: Payment = {
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
  const result = await Xrpld.submit(testContext.client, {
    wallet: fromWallet,
    tx: builtTx,
  })
  const hookExecutions = await ExecutionUtility.getHookExecutionsFromMeta(
    testContext.client,
    result.meta as TransactionMetadata
  )
  expect(hookExecutions.executions[1].HookReturnString).toEqual(
    'pet_race.c: Pet added to race.'
  )
}

describe('Pets - Success Group', () => {
  let testContext: XrplIntegrationTestContext

  beforeAll(async () => {
    testContext = await setupClient(serverUrl)
    const hookWallet = testContext.hook1

    const gwWallet = testContext.gw
    const USD = IC.gw('USD', gwWallet.classicAddress)
    await trust(testContext.client, USD.set(100000), ...[hookWallet])

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
    console.log(
      JSON.stringify([
        { Hook: {} },
        { Hook: {} },
        { Hook: {} },
        { Hook: {} },
        { Hook: hook5 },
        { Hook: hook6 },
      ])
    )
    await setHooksV3({
      client: testContext.client,
      seed: testContext.hook1.seed,
      hooks: [
        { Hook: {} },
        { Hook: {} },
        { Hook: {} },
        { Hook: {} },
        { Hook: hook5 },
        { Hook: hook6 },
      ],
    } as SetHookParams)
    const dns = hexNamespace(`/races/${1}`)
    const clearHook = {
      Flags: SetHookFlags.hsfNSDelete,
      HookNamespace: dns,
    } as iHook
    await clearHookStateV3({
      client: testContext.client,
      seed: testContext.hook1.seed,
      hooks: [{ Hook: clearHook }],
    } as SetHookParams)
  })
  afterAll(async () => teardownClient(testContext))

  it('create race', async () => {
    const bobWallet = testContext.bob
    const hookWallet = testContext.hook1

    const raceId = 1

    const CLOSE_TIME: number = (
      await testContext.client.request({
        command: 'ledger',
        ledger_index: 'validated',
      })
    ).result.ledger.close_time

    const testRace = new RaceModel(
      BigInt(raceId),
      'Saratoga Springs',
      0,
      0,
      0,
      1,
      5,
      10,
      CLOSE_TIME + 10,
      100,
      0.5,
      0.5
    )

    const ns = hexNamespace(`/races`)
    console.log(`/races: ${ns}`)

    const hash = hexNamespace(`/races/${raceId}`)
    console.log(`/races/${raceId}: ${hash}`)

    const dpns = hexNamespace(`/races/${raceId}/pool`)
    console.log(`/races/${raceId}/pool: ${dpns}`)

    const txParam1 = new iHookParamEntry(
      new iHookParamName('HPA'),
      new iHookParamValue('0A01000000010000000000', true)
    )
    const txParam2 = new iHookParamEntry(
      new iHookParamName('RCN'),
      new iHookParamValue(ns, true)
    )
    const txParam3 = new iHookParamEntry(
      new iHookParamName('RH'),
      new iHookParamValue(hash, true)
    )

    const txParam4 = new iHookParamEntry(
      new iHookParamName('RM'),
      new iHookParamValue(testRace.encode(), true)
    )
    const BOB_ID: string = (
      (await testContext.client.request({
        command: 'account_objects',
        account: testContext.bob.classicAddress,
        type: 'uri_token',
      })) as AccountObjectsResponse
    ).result.account_objects[0].index
    console.log(BOB_ID)
    const txParam5 = new iHookParamEntry(
      new iHookParamName('PH'),
      new iHookParamValue(BOB_ID, true)
    )
    const txParam6 = new iHookParamEntry(
      new iHookParamName('RPN'),
      new iHookParamValue(dpns, true)
    )
    const amount: IssuedCurrencyAmount = {
      issuer: testContext.hook1.classicAddress,
      currency: 'PXP',
      value: '100',
    }
    const builtTx1: Payment = {
      TransactionType: 'Payment',
      Account: bobWallet.classicAddress,
      Destination: hookWallet.classicAddress,
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
    console.log(JSON.stringify(builtTx1))

    const result1 = await Xrpld.submit(testContext.client, {
      wallet: bobWallet,
      tx: builtTx1,
    })

    const hookExecutions1 = await ExecutionUtility.getHookExecutionsFromMeta(
      testContext.client,
      result1.meta as TransactionMetadata
    )
    expect(hookExecutions1.executions[1].HookReturnString).toEqual(
      'pet_race.c: Pet added to race.'
    )
  })

  it('add pet to race', async () => {
    const CAROL_ID: string = (
      (await testContext.client.request({
        command: 'account_objects',
        account: testContext.carol.classicAddress,
        type: 'uri_token',
      })) as AccountObjectsResponse
    ).result.account_objects[0].index
    await addPetToRace(
      testContext,
      testContext.carol,
      testContext.hook1.classicAddress,
      1,
      CAROL_ID
    )
    // const DAVE_ID: string = (
    //   (await testContext.client.request({
    //     command: 'account_objects',
    //     account: testContext.dave.classicAddress,
    //     type: 'uri_token',
    //   })) as AccountObjectsResponse
    // ).result.account_objects[0].index
    // await addPetToRace(
    //   testContext,
    //   testContext.dave,
    //   testContext.hook1.classicAddress,
    //   1,
    //   DAVE_ID
    // )
    // const ELSA_ID: string = (
    //   (await testContext.client.request({
    //     command: 'account_objects',
    //     account: testContext.elsa.classicAddress,
    //     type: 'uri_token',
    //   })) as AccountObjectsResponse
    // ).result.account_objects[0].index
    // await addPetToRace(
    //   testContext,
    //   testContext.elsa,
    //   testContext.hook1.classicAddress,
    //   1,
    //   ELSA_ID
    // )
  })
})
