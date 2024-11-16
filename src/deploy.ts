import {
  Client,
  // Wallet,
  SetHookFlags,
} from '@transia/xrpl'
import {
  createHookPayload,
  //   setHooksV3,
  //   SetHookParams,
  iHookParamEntry,
  iHookParamName,
  iHookParamValue,
  SetHookPayload,
  floatToLEXfl,
} from '@transia/hooks-toolkit'
import * as dotenv from 'dotenv'
import path from 'path'
import fs from 'fs'
dotenv.config({ path: path.resolve(__dirname, '..', '.env.private') })

export async function set(): Promise<void> {
  const serverUrl = 'wss://xahau.network'
  const client = new Client(serverUrl)
  await client.connect()
  client.networkID = await client.getNetworkID()

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
  // console.log(
  //   JSON.stringify([
  //     { Hook: hook1 },
  //     { Hook: hook2 },
  //     { Hook: hook3 },
  //     { Hook: hook4 },
  //   ])
  // )

  // write to json file
  const builtTxn = {
    TransactionType: 'SetHook',
    Hooks: [{ Hook: hook1 }, { Hook: hook2 }, { Hook: hook3 }, { Hook: hook4 }],
    NetworkID: 21337,
  }
  fs.writeFileSync('deploy.json', JSON.stringify(builtTxn, null, 2))

  //   const hookWallet = Wallet.fromSeed(process.env.ISSUER_SEED || '')
  //   console.log(hookWallet)
  //   await setHooksV3({
  //     client: client,
  //     seed: hookWallet.seed,
  //     hooks: [{ Hook: hook1 }, { Hook: hook2 }, { Hook: hook3 }, { Hook: hook4 }],
  //   } as SetHookParams)
  await client.disconnect()
}

set()
