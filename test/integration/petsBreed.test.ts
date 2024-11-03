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

    await mintPet(
      testContext,
      aliceWallet,
      'Damascus',
      hookWallet.classicAddress
    )
    await mintPet(
      testContext,
      bobWallet,
      'Lawyer Ron',
      hookWallet.classicAddress
    )
    await mintPet(
      testContext,
      testContext.carol,
      'Serenas Song',
      hookWallet.classicAddress
    )
    await mintPet(
      testContext,
      testContext.dave,
      'Lava Man',
      hookWallet.classicAddress
    )
    await mintPet(
      testContext,
      testContext.elsa,
      'Exterminator',
      hookWallet.classicAddress
    )
    await mintPet(
      testContext,
      testContext.frank,
      'Arrogate',
      hookWallet.classicAddress
    )

    try {
      await mintPet(
        testContext,
        testContext.frank,
        'Arrogate 2x',
        hookWallet.classicAddress
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
