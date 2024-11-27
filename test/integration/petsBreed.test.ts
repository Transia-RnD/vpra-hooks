import { SetHookFlags } from '@transia/xrpl'
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
import { getPet, updatePet } from './utils'
import { breedPet } from './utils'
// import { mintPet } from './utils'
// import { URIToken } from '@transia/xrpl/dist/npm/models/ledger'

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

    // await mintPet(
    //   testContext,
    //   aliceWallet,
    //   100,
    //   'Damascus',
    //   hookWallet.classicAddress,
    //   'pet_mint.c: Finished.'
    // )
    // await mintPet(
    //   testContext,
    //   bobWallet,
    //   100,
    //   'Lawyer Ron',
    //   hookWallet.classicAddress,
    //   'pet_mint.c: Finished.'
    // )
    // await mintPet(
    //   testContext,
    //   testContext.carol,
    //   100,
    //   'Serenas Song',
    //   hookWallet.classicAddress,
    //   'pet_mint.c: Finished.'
    // )
    // await mintPet(
    //   testContext,
    //   testContext.dave,
    //   100,
    //   'Lava Man',
    //   hookWallet.classicAddress,
    //   'pet_mint.c: Finished.'
    // )
    // await mintPet(
    //   testContext,
    //   testContext.elsa,
    //   100,
    //   'Exterminator',
    //   hookWallet.classicAddress,
    //   'pet_mint.c: Finished.'
    // )
    // await mintPet(
    //   testContext,
    //   testContext.frank,
    //   100,
    //   'Arrogate',
    //   hookWallet.classicAddress,
    //   'pet_mint.c: Finished.'
    // )

    // const ALICE_URITOKEN: URIToken = (
    //   (await testContext.client.request({
    //     command: 'account_objects',
    //     account: testContext.alice.classicAddress,
    //     type: 'uri_token',
    //   })) as AccountObjectsResponse
    // ).result.account_objects[0] as URIToken

    // const maleID = ALICE_URITOKEN.Digest
    // const maleHash = ALICE_URITOKEN.index
    const maleID =
      'D40CE668F7B9EFBE73C8A146D78DBF1BBA6E1F454078FBC40E598E94EF1EE5B1'
    const maleHash =
      '11C467069B48E6A2A32809FAEBF91DE5E859BA4FC2B1D24EDD82AFBFDD9DCC1B'

    // const BOB_URITOKEN: URIToken = (
    //   (await testContext.client.request({
    //     command: 'account_objects',
    //     account: testContext.bob.classicAddress,
    //     type: 'uri_token',
    //   })) as AccountObjectsResponse
    // ).result.account_objects[1] as URIToken
    // const femaleID = BOB_URITOKEN.Digest
    // const femaleHash = BOB_URITOKEN.index
    const femaleID =
      '3F9EA9EB8A32EC06C0BD086C961B5E62D73C6ECAE42082A1D6E1DF3906BCEDC0'
    const femaleHash =
      '389722E386F06F8E646372EA4DA18CABE91FCAA7E0A7D576A3F7D2B8C9153BB0'

    // UPDATE MALE PET
    await updatePet(
      testContext.client,
      maleHash,
      aliceWallet,
      hookWallet.classicAddress,
      '10'
    )

    {
      const pet = await getPet(
        testContext.client,
        hookWallet.classicAddress,
        maleID
      )
      console.log(pet)
    }

    // UPDATE FEMALE PET
    await updatePet(
      testContext.client,
      femaleHash,
      bobWallet,
      hookWallet.classicAddress,
      '10'
    )

    {
      const pet = await getPet(
        testContext.client,
        hookWallet.classicAddress,
        femaleID
      )
      console.log(pet)
    }

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

    // {
    //   const pet = await getPet(
    //     testContext.client,
    //     hookWallet.classicAddress,
    //     maleHash
    //   )
    //   expect(pet.isBreedable).toBe(0)
    //   expect(pet.breedPrice).toBe(0)
    // }
  })
})
