import { SetHookFlags, AccountObjectsResponse } from '@transia/xrpl'
// xrpl-helpers
import {
  serverUrl,
  XrplIntegrationTestContext,
  setupClient,
  teardownClient,
  close,
} from '@transia/hooks-toolkit/dist/npm/src/libs/xrpl-helpers'
// src
import {
  SetHookParams,
  createHookPayload,
  setHooksV3,
  SetHookPayload,
  iHookParamEntry,
  iHookParamName,
  iHookParamValue,
  floatToLEXfl,
} from '@transia/hooks-toolkit'
import { breedPet, buyPet, mintPet, sellPet, updatePet } from './utils'

describe('Pets - Success Group', () => {
  let testContext: XrplIntegrationTestContext

  beforeAll(async () => {
    testContext = await setupClient(serverUrl)

    const hook1 = createHookPayload({
      version: 0,
      createFile: 'router_base',
      namespace: 'pets',
      flags: SetHookFlags.hsfOverride,
      hookOnArray: ['Payment', 'Invoke'],
    } as SetHookPayload)
    const hook2Param1 = new iHookParamEntry(
      new iHookParamName('MP'),
      new iHookParamValue(floatToLEXfl('100'), true)
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
    console.log(
      JSON.stringify([
        { Hook: hook1 },
        { Hook: hook2 },
        { Hook: hook3 },
        { Hook: hook4 },
      ])
    )

    await setHooksV3({
      client: testContext.client,
      seed: testContext.hook1.seed,
      hooks: [
        { Hook: hook1 },
        { Hook: hook2 },
        { Hook: hook3 },
        { Hook: hook4 },
      ],
    } as SetHookParams)
  })
  afterAll(async () => teardownClient(testContext))

  it('pets', async () => {
    const hookWallet = testContext.hook1
    const aliceWallet = testContext.alice
    const bobWallet = testContext.bob

    try {
      await mintPet(
        testContext,
        aliceWallet,
        99,
        'Damascus',
        hookWallet.classicAddress,
        ''
      )
    } catch (error: any) {
      console.log(error)
      expect(JSON.parse(error.message)[1].HookReturnString).toEqual(
        'pet_mint.c: Insufficient Payment `Amount`'
      )
    }
    try {
      await mintPet(
        testContext,
        aliceWallet,
        100,
        'My Name Is Long',
        hookWallet.classicAddress,
        ''
      )
    } catch (error: any) {
      expect(error.message).toEqual('String length 15 exceeds max length of 13')
    }

    await mintPet(
      testContext,
      aliceWallet,
      100,
      'Damascus',
      hookWallet.classicAddress,
      'pet_mint.c: Finished.'
    )
    await mintPet(
      testContext,
      bobWallet,
      100,
      'Lawyer Ron',
      hookWallet.classicAddress,
      'pet_mint.c: Finished.'
    )
    await mintPet(
      testContext,
      testContext.carol,
      100,
      'Serenas Song',
      hookWallet.classicAddress,
      'pet_mint.c: Finished.'
    )
    await mintPet(
      testContext,
      testContext.dave,
      100,
      'Lava Man',
      hookWallet.classicAddress,
      'pet_mint.c: Finished.'
    )
    await mintPet(
      testContext,
      testContext.elsa,
      100,
      'Exterminator',
      hookWallet.classicAddress,
      'pet_mint.c: Finished.'
    )
    await mintPet(
      testContext,
      testContext.frank,
      100,
      'Arrogate',
      hookWallet.classicAddress,
      'pet_mint.c: Finished.'
    )

    try {
      await mintPet(
        testContext,
        testContext.frank,
        100,
        'Arrogate 2x',
        hookWallet.classicAddress,
        'pet_mint.c: Finished.'
      )
    } catch (error) {
      console.log(error)
    }

    const ALICE_ID: string = (
      (await testContext.client.request({
        command: 'account_objects',
        account: testContext.alice.classicAddress,
        type: 'uri_token',
      })) as AccountObjectsResponse
    ).result.account_objects[0].index

    const maleHash = ALICE_ID

    const BOB_ID: string = (
      (await testContext.client.request({
        command: 'account_objects',
        account: testContext.bob.classicAddress,
        type: 'uri_token',
      })) as AccountObjectsResponse
    ).result.account_objects[0].index
    const femaleHash = BOB_ID

    // UPDATE MALE PET
    await updatePet(
      testContext.client,
      maleHash,
      aliceWallet,
      hookWallet.classicAddress,
      floatToLEXfl('10')
    )

    // UPDATE FEMALE PET
    await updatePet(
      testContext.client,
      femaleHash,
      bobWallet,
      hookWallet.classicAddress,
      floatToLEXfl('10')
    )

    // BREED FEMALE PET
    await breedPet(
      testContext.client,
      bobWallet,
      hookWallet.classicAddress,
      maleHash,
      femaleHash,
      '10'
    )
    await close(testContext.client)

    // BUY / SELL
    await sellPet(testContext.client, aliceWallet, maleHash, '1')
    await buyPet(testContext.client, bobWallet, maleHash, '1')
  })
})
