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
} from '@transia/hooks-toolkit'
import { varStringToHex } from '@transia/hooks-toolkit/dist/npm/src/libs/binary-models'

export async function mintPet(
  testContext: XrplIntegrationTestContext,
  fromWallet: Wallet,
  name: string,
  destination: string
) {
  const otxn1param1 = new iHookParamEntry(
    new iHookParamName('HPA'),
    new iHookParamValue('0A01010000000000000000', true)
  )
  const otxn1param2 = new iHookParamEntry(
    new iHookParamName('PN'),
    new iHookParamValue(varStringToHex(name, 31), true)
  )
  const builtTx: Payment = {
    TransactionType: 'Payment',
    Account: fromWallet.classicAddress,
    Destination: destination,
    Amount: xrpToDrops(10),
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
  expect(hookExecutions.executions[1].HookReturnString).toEqual(
    'pet_mint.c: Finished.'
  )

  const PXP = IC.gw('PXP', testContext.hook1.classicAddress)
  await trust(testContext.client, PXP.set(100000), ...[fromWallet])
}

export async function updatePet(
  client: Client,
  hash: string,
  wallet: Wallet,
  dest: string,
  xfl: string
) {
  const otxnparam1 = new iHookParamEntry(
    new iHookParamName('HPA'),
    new iHookParamValue('0A01000100000000000000', true)
  )
  const otxnparam2 = new iHookParamEntry(
    new iHookParamName('P'),
    new iHookParamValue(hash, true)
  )
  const otxnparam3 = new iHookParamEntry(
    new iHookParamName('BP'),
    new iHookParamValue(xfl, true)
  )
  // const otxnparam4 = new iHookParamEntry(
  //   new iHookParamName('PN'),
  //   new iHookParamValue(varStringToHex('A Sons Love', 31), true)
  // )
  const builtTx3: Invoke = {
    TransactionType: 'Invoke',
    Account: wallet.classicAddress,
    Destination: dest,
    HookParameters: [
      otxnparam1.toXrpl(),
      otxnparam2.toXrpl(),
      otxnparam3.toXrpl(),
      // otxnparam4.toXrpl(),
    ],
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
    new iHookParamValue(varStringToHex('A Sons Love', 31), true)
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
