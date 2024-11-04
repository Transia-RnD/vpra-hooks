import { SetHookFlags, AccountObjectsResponse } from '@transia/xrpl'
// xrpl-helpers
import {
  serverUrl,
  XrplIntegrationTestContext,
  setupClient,
  teardownClient,
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
import { getPet, mintPet } from './utils'
import { URIToken } from '@transia/xrpl/dist/npm/models/ledger'

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

  it('mint pet', async () => {
    const hookWallet = testContext.hook1
    const aliceWallet = testContext.alice
    const mintVersion = 0
    // const initalResponse = (await testContext.client.request({
    //   command: 'account_objects',
    //   account: testContext.alice.classicAddress,
    //   type: 'uri_token',
    // })) as AccountObjectsResponse

    // if (initalResponse.result.account_objects.length === 0) {
    //   await mintPet(
    //     testContext,
    //     aliceWallet,
    //     100,
    //     'Damascus',
    //     hookWallet.classicAddress,
    //     'pet_mint.c: Finished.'
    //   )
    // }

    await mintPet(
      testContext,
      aliceWallet,
      100,
      'Damascus',
      hookWallet.classicAddress,
      'pet_mint.c: Finished.'
    )

    const ALICE_URITOKEN: URIToken = (
      (await testContext.client.request({
        command: 'account_objects',
        account: testContext.alice.classicAddress,
        type: 'uri_token',
      })) as AccountObjectsResponse
    ).result.account_objects[mintVersion] as URIToken

    const maleHash = ALICE_URITOKEN.Digest

    {
      const pet = await getPet(
        testContext.client,
        hookWallet.classicAddress,
        maleHash
      )
      expect(pet.isBreedable).toBe(0)
      expect(pet.breedPrice).toBe(0)
    }
  })
})
