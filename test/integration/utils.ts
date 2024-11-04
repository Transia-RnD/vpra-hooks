import {
  Invoke,
  Payment,
  TransactionMetadata,
  Client,
  Wallet,
  xrpToDrops,
  URITokenCreateSellOffer,
  URITokenBuy,
} from '@transia/xrpl'
// xrpl-helpers
import {
  XrplIntegrationTestContext,
  trust,
  IC,
  close,
} from '@transia/hooks-toolkit/dist/npm/src/libs/xrpl-helpers'
// src
import {
  Xrpld,
  ExecutionUtility,
  iHookParamEntry,
  iHookParamName,
  iHookParamValue,
  floatToLEXfl,
  StateUtility,
  hexNamespace,
} from '@transia/hooks-toolkit'
import {
  decodeModel,
  varStringToHex,
} from '@transia/hooks-toolkit/dist/npm/src/libs/binary-models'
import { PetModel } from './models/PetModel'

export async function getPet(
  client: Client,
  account: string,
  petHash: string
): Promise<PetModel | null> {
  try {
    const result = await StateUtility.getHookState(
      client,
      account,
      petHash,
      hexNamespace(`pets`)
    )
    const battle = decodeModel(result.HookStateData, PetModel)
    console.log(battle)
    return battle
  } catch (e) {
    console.log(e)
    return null
  }
}

export async function mintPet(
  testContext: XrplIntegrationTestContext,
  fromWallet: Wallet,
  amount: number,
  name: string,
  destination: string,
  executionResult: string
) {
  const otxn1param1 = new iHookParamEntry(
    new iHookParamName('HPA'),
    new iHookParamValue('0A01010000000000000000', true)
  )
  const otxn1param2 = new iHookParamEntry(
    new iHookParamName('PN'),
    new iHookParamValue(varStringToHex(name, 13), true)
  )
  const builtTx: Payment = {
    TransactionType: 'Payment',
    Account: fromWallet.classicAddress,
    Destination: destination,
    Amount: xrpToDrops(amount),
    HookParameters: [otxn1param1.toXrpl(), otxn1param2.toXrpl()],
  }
  const result = await Xrpld.submit(testContext.client, {
    wallet: fromWallet,
    tx: builtTx,
  })

  const hookExecutions = await ExecutionUtility.getHookExecutionsFromMeta(
    testContext.client,
    result.meta as TransactionMetadata
  )
  await close(testContext.client)
  expect(hookExecutions.executions[1].HookReturnString).toEqual(executionResult)

  const PXP = IC.gw('PXP', testContext.hook1.classicAddress)
  await trust(testContext.client, PXP.set(100000), ...[fromWallet])
}

export async function updatePet(
  client: Client,
  hash: string,
  wallet: Wallet,
  dest: string,
  breedPrice?: string,
  name?: string
) {
  const otxnparam1 = new iHookParamEntry(
    new iHookParamName('HPA'),
    new iHookParamValue('0A01000100000000000000', true)
  )
  const otxnparam2 = new iHookParamEntry(
    new iHookParamName('P'),
    new iHookParamValue(hash, true)
  )
  const builtTx3: Invoke = {
    TransactionType: 'Invoke',
    Account: wallet.classicAddress,
    Destination: dest,
    HookParameters: [otxnparam1.toXrpl(), otxnparam2.toXrpl()],
  }
  if (breedPrice) {
    const otxnparam = new iHookParamEntry(
      new iHookParamName('BP'),
      new iHookParamValue(floatToLEXfl(breedPrice), true)
    )
    builtTx3.HookParameters.push(otxnparam.toXrpl())
  }
  if (name) {
    const otxnparam = new iHookParamEntry(
      new iHookParamName('PN'),
      new iHookParamValue(varStringToHex(name, 13), true)
    )
    builtTx3.HookParameters.push(otxnparam.toXrpl())
  }

  await Xrpld.submit(client, {
    wallet: wallet,
    tx: builtTx3,
  })
}

export async function breedPet(
  client: Client,
  wallet: Wallet,
  dest: string,
  maleHash: string,
  femaleHash: string,
  amount: string
) {
  const otxnparam1 = new iHookParamEntry(
    new iHookParamName('HPA'),
    new iHookParamValue('0A01000001000000000000', true)
  )
  const otxnparam2 = new iHookParamEntry(
    new iHookParamName('PF'),
    new iHookParamValue(femaleHash, true)
  )
  const otxnparam3 = new iHookParamEntry(
    new iHookParamName('PM'),
    new iHookParamValue(maleHash, true)
  )
  const otxnparam4 = new iHookParamEntry(
    new iHookParamName('PN'),
    new iHookParamValue(varStringToHex('A Sons Love', 13), true)
  )
  const builtTx4: Payment = {
    TransactionType: 'Payment',
    Account: wallet.classicAddress,
    Destination: dest,
    Amount: xrpToDrops(amount),
    HookParameters: [
      otxnparam1.toXrpl(),
      otxnparam2.toXrpl(),
      otxnparam3.toXrpl(),
      otxnparam4.toXrpl(),
    ],
  }
  await Xrpld.submit(client, {
    wallet: wallet,
    tx: builtTx4,
  })
}

export async function sellPet(
  client: Client,
  wallet: Wallet,
  tokenId: string,
  amount: string
) {
  const builtTx4: URITokenCreateSellOffer = {
    TransactionType: 'URITokenCreateSellOffer',
    Account: wallet.classicAddress,
    URITokenID: tokenId,
    Amount: xrpToDrops(amount),
  }
  await Xrpld.submit(client, {
    wallet: wallet,
    tx: builtTx4,
  })
}

export async function buyPet(
  client: Client,
  wallet: Wallet,
  tokenId: string,
  amount: string
) {
  const builtTx4: URITokenBuy = {
    TransactionType: 'URITokenBuy',
    Account: wallet.classicAddress,
    URITokenID: tokenId,
    Amount: xrpToDrops(amount),
  }
  await Xrpld.submit(client, {
    wallet: wallet,
    tx: builtTx4,
  })
}
