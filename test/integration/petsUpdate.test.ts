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
import { getPet, updatePet } from './utils'
// import { getPet, mintPet, updatePet } from './utils'
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

  it('update pet', async () => {
    const hookWallet = testContext.hook1
    const aliceWallet = testContext.alice

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
    ).result.account_objects[0] as URIToken

    const maleID = ALICE_URITOKEN.index
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

    // SET PET BREEDABLE
    await updatePet(
      testContext.client,
      maleID,
      aliceWallet,
      hookWallet.classicAddress,
      String(10)
    )

    {
      const pet = await getPet(
        testContext.client,
        hookWallet.classicAddress,
        maleHash
      )
      expect(pet.isBreedable).toBe(1)
      expect(pet.breedPrice).toBe(10)
    }

    // SET PET NOT BREEDABLE
    await updatePet(
      testContext.client,
      maleID,
      aliceWallet,
      hookWallet.classicAddress,
      String(0)
    )

    {
      const pet = await getPet(
        testContext.client,
        hookWallet.classicAddress,
        maleHash
      )
      expect(pet.isBreedable).toBe(0)
      expect(pet.breedPrice).toBe(0)
    }

    // SET PET NAME
    await updatePet(
      testContext.client,
      maleID,
      aliceWallet,
      hookWallet.classicAddress,
      String(10),
      'Fido'
    )

    {
      const pet = await getPet(
        testContext.client,
        hookWallet.classicAddress,
        maleHash
      )
      expect(pet.isBreedable).toBe(1)
      expect(pet.breedPrice).toBe(10)
      expect(pet.name).toBe('Fido')
    }
  })
})
