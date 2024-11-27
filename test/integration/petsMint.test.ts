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
    ).result.account_objects[5] as URIToken

    const maleHash = ALICE_URITOKEN.Digest

    {
      const pet = await getPet(
        testContext.client,
        hookWallet.classicAddress,
        maleHash
      )
      console.log(pet)
      // expect(pet.id).toEqual(0n)
      expect(pet.name).toEqual('Damascus')
      expect(pet.gender === 0 || pet.gender === 1).toBeTruthy()
      expect(pet.age).toEqual(0)
      expect(pet.breed >= 0 && pet.breed <= 3).toBeTruthy()
      expect(pet.size >= 1 && pet.size <= 10).toBeTruthy()
      expect(pet.body >= 1 && pet.body <= 10).toBeTruthy()
      const speedOptions = [55, 60, 65, 70, 75, 80, 85, 90, 95, 100]
      expect(speedOptions.includes(pet.speed)).toBeTruthy()
      expect(pet.stamina >= 1 && pet.stamina <= 10).toBeTruthy()
      expect(pet.temperament >= 1 && pet.temperament <= 10).toBeTruthy()
      expect(pet.training >= 1 && pet.training <= 10).toBeTruthy()
      expect(pet.health >= 1 && pet.health <= 10).toBeTruthy()
      const lifespanOptions = [10, 15, 20, 25, 30, 35, 40, 45, 50, 55]
      expect(lifespanOptions.includes(pet.lifespan)).toBeTruthy()
      expect(pet.affinity >= 0 && pet.affinity <= 100).toBeTruthy()
      expect(pet.morale).toEqual(0n)
      expect(pet.isBreedable).toEqual(0)
      expect(pet.breedPrice).toEqual(0)
      expect(pet.tokenId).not.toBeNull()
      expect(pet.wins).toEqual(0)
      expect(pet.total).toEqual(0n)
    }

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
  })
})
