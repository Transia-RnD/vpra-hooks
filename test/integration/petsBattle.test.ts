import {
  Wallet,
  Payment,
  Invoke,
  SetHookFlags,
  TransactionMetadata,
  AccountObjectsResponse,
  Client,
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
  hexNamespace,
  floatToLEXfl,
  StateUtility,
  decodeModel,
} from '@transia/hooks-toolkit'
import { IssuedCurrencyAmount } from '@transia/xrpl/dist/npm/models/common'
import { BattleModel } from './models/BattleModel'
import { mintPet } from './utils'

export async function getBattle(
  client: Client,
  account: string,
  battleHash: string
): Promise<BattleModel | null> {
  try {
    console.log(account)
    console.log(battleHash)

    const result = await StateUtility.getHookState(
      client,
      account,
      battleHash,
      hexNamespace(`/battles`)
    )
    const battle = decodeModel(result.HookStateData, BattleModel)
    console.log(battle)
    return battle
  } catch (e) {
    console.log(e)
    return null
  }
}

export async function createBattle(
  client: Client,
  fromWallet: Wallet,
  destination: string,
  battle: BattleModel,
  petHash: string
) {
  // Collection Name Space
  const cns = hexNamespace(`/battles`)

  // Document Name Space
  const dns = hexNamespace(`/battles/${battle.id}`)

  const txParam1 = new iHookParamEntry(
    new iHookParamName('HPA'),
    new iHookParamValue('0A01000000010000000000', true)
  )
  const txParam2 = new iHookParamEntry(
    new iHookParamName('OP'),
    new iHookParamValue('C')
  )
  const txParam3 = new iHookParamEntry(
    new iHookParamName('BCN'),
    new iHookParamValue(cns, true)
  )
  const txParam4 = new iHookParamEntry(
    new iHookParamName('BH'),
    new iHookParamValue(dns, true)
  )
  const txParam5 = new iHookParamEntry(
    new iHookParamName('BM'),
    new iHookParamValue(battle.encode(), true)
  )
  const txParam6 = new iHookParamEntry(
    new iHookParamName('PH'),
    new iHookParamValue(petHash, true)
  )
  const amount: IssuedCurrencyAmount = {
    issuer: destination,
    currency: 'PXP',
    value: '10',
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
      txParam6.toXrpl(),
    ],
  }
  const result = await Xrpld.submit(client, {
    wallet: fromWallet,
    tx: builtTx,
  })
  const hookExecutions = await ExecutionUtility.getHookExecutionsFromMeta(
    client,
    result.meta as TransactionMetadata
  )
  expect(hookExecutions.executions[1].HookReturnString).toEqual(
    'pet_battle.c: Battle Created.'
  )
}

export async function joinBattle(
  client: Client,
  fromWallet: Wallet,
  destination: string,
  battleHash: string,
  petHash: string
) {
  const txParam1 = new iHookParamEntry(
    new iHookParamName('HPA'),
    new iHookParamValue('0A01000000010000000000', true)
  )
  const txParam2 = new iHookParamEntry(
    new iHookParamName('OP'),
    new iHookParamValue('J')
  )
  const txParam3 = new iHookParamEntry(
    new iHookParamName('BH'),
    new iHookParamValue(battleHash, true)
  )
  const txParam4 = new iHookParamEntry(
    new iHookParamName('PH'),
    new iHookParamValue(petHash, true)
  )
  const amount: IssuedCurrencyAmount = {
    issuer: destination,
    currency: 'PXP',
    value: '10',
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
    ],
  }
  const result = await Xrpld.submit(client, {
    wallet: fromWallet,
    tx: builtTx,
  })
  const hookExecutions = await ExecutionUtility.getHookExecutionsFromMeta(
    client,
    result.meta as TransactionMetadata
  )
  expect(hookExecutions.executions[1].HookReturnString).toEqual(
    'pet_battle.c: Battle Joined.'
  )
}

export async function rollDice(
  client: Client,
  fromWallet: Wallet,
  destination: string,
  battleHash: string,
  petHash: string
) {
  const txParam1 = new iHookParamEntry(
    new iHookParamName('HPA'),
    new iHookParamValue('0A01000000010000000000', true)
  )
  const txParam2 = new iHookParamEntry(
    new iHookParamName('OP'),
    new iHookParamValue('R')
  )
  const txParam3 = new iHookParamEntry(
    new iHookParamName('BH'),
    new iHookParamValue(battleHash, true)
  )
  const txParam4 = new iHookParamEntry(
    new iHookParamName('PH'),
    new iHookParamValue(petHash, true)
  )
  const builtTx: Invoke = {
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
  const result = await Xrpld.submit(client, {
    wallet: fromWallet,
    tx: builtTx,
  })
  const hookExecutions = await ExecutionUtility.getHookExecutionsFromMeta(
    client,
    result.meta as TransactionMetadata
  )
  expect(hookExecutions.executions[1].HookReturnString).toEqual(
    'pet_battle.c: Dice Rolled.'
  )
}

export async function endBattle(
  client: Client,
  fromWallet: Wallet,
  destination: string,
  battleHash: string
) {
  const txParam1 = new iHookParamEntry(
    new iHookParamName('HPA'),
    new iHookParamValue('0A01000000010000000000', true)
  )
  const txParam2 = new iHookParamEntry(
    new iHookParamName('OP'),
    new iHookParamValue('E')
  )
  const txParam3 = new iHookParamEntry(
    new iHookParamName('BH'),
    new iHookParamValue(battleHash, true)
  )
  const builtTx: Invoke = {
    TransactionType: 'Invoke',
    Account: fromWallet.classicAddress,
    Destination: destination,
    HookParameters: [txParam1.toXrpl(), txParam2.toXrpl(), txParam3.toXrpl()],
  }
  const result = await Xrpld.submit(client, {
    wallet: fromWallet,
    tx: builtTx,
  })
  const hookExecutions = await ExecutionUtility.getHookExecutionsFromMeta(
    client,
    result.meta as TransactionMetadata
  )
  expect(hookExecutions.executions[1].HookReturnString).toEqual(
    'pet_battle.c: Battle Ended.'
  )
}

export const getURITokenID = async (client: Client, address: string) => {
  const uritokens = (
    (await client.request({
      command: 'account_objects',
      account: address,
      type: 'uri_token',
    })) as AccountObjectsResponse
  ).result.account_objects
  if (uritokens.length === 0) {
    return null
  }
  return uritokens[0].index
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
      createFile: 'pet_battleV2',
      namespace: '/battles',
      flags: SetHookFlags.hsfOverride,
      hookOnArray: ['Payment', 'Invoke'],
    } as SetHookPayload)
    // console.log(
    //   JSON.stringify([
    //     { Hook: hook1 },
    //     { Hook: hook2 },
    //     { Hook: hook3 },
    //     { Hook: hook4 },
    //   ])
    // )
    await setHooksV3({
      client: testContext.client,
      seed: testContext.hook1.seed,
      hooks: [
        { Hook: hook1 },
        { Hook: hook2 },
        { Hook: hook3 },
        { Hook: hook4 },
        { Hook: hook5 },
      ],
    } as SetHookParams)
  })
  afterAll(async () => teardownClient(testContext))

  it('battle pets', async () => {
    const hookWallet = testContext.hook1
    const aliceWallet = testContext.alice
    const bobWallet = testContext.bob

    // MINT PETS
    if (
      (await getURITokenID(testContext.client, aliceWallet.classicAddress)) ===
      null
    ) {
      await mintPet(
        testContext,
        aliceWallet,
        'Damascus',
        hookWallet.classicAddress
      )
    }
    if (
      (await getURITokenID(testContext.client, bobWallet.classicAddress)) ===
      null
    ) {
      await mintPet(
        testContext,
        bobWallet,
        'Lawyer Ron',
        hookWallet.classicAddress
      )
    }

    const aliceID = await getURITokenID(
      testContext.client,
      aliceWallet.classicAddress
    )
    const bobID = await getURITokenID(
      testContext.client,
      bobWallet.classicAddress
    )

    // CREATE BATTLE
    const battle = new BattleModel(
      'CustomID',
      0,
      '0000000000000000000000000000000000000000000000000000000000000000',
      'rrrrrrrrrrrrrrrrrrrrrhoLvTp',
      0xff,
      '0000000000000000000000000000000000000000000000000000000000000000',
      'rrrrrrrrrrrrrrrrrrrrrhoLvTp',
      0xff
    )
    const battleHash = hexNamespace(`/battles/${battle.id}`)
    if (
      (await getBattle(
        testContext.client,
        hookWallet.classicAddress,
        battleHash
      )) === null
    ) {
      console.log(battle.encode().length / 2)
      await createBattle(
        testContext.client,
        aliceWallet,
        hookWallet.classicAddress,
        battle,
        aliceID
      )
    }

    // JOIN BATTLE
    console.log(battleHash)
    console.log(bobID)

    // await joinBattle(
    //   testContext.client,
    //   bobWallet,
    //   hookWallet.classicAddress,
    //   battleHash,
    //   bobID
    // )

    // // ROLL DICE (Attacker)
    // await rollDice(
    //   testContext.client,
    //   bobWallet,
    //   hookWallet.classicAddress,
    //   battleHash,
    //   bobID
    // )

    // // ROLL DICE (Defender)
    // await rollDice(
    //   testContext.client,
    //   aliceWallet,
    //   hookWallet.classicAddress,
    //   battleHash,
    //   aliceID
    // )

    // END BATTLE
    await endBattle(
      testContext.client,
      aliceWallet,
      hookWallet.classicAddress,
      battleHash
    )
  })
})
