import {
  BaseModel,
  Hash256,
  Metadata,
  UInt8,
  VarString,
  XRPAddress,
} from '@transia/hooks-toolkit/dist/npm/src/libs/binary-models'

export class BattleModel extends BaseModel {
  id: VarString
  battleType: UInt8 // 0: Attribute, 1: Pet
  defenderHash: Hash256
  defenderAddress: XRPAddress
  defenderRoll: UInt8
  attackerHash: Hash256
  attackerAddress: XRPAddress
  attackerRoll: UInt8

  // 139 bytes
  constructor(
    id: VarString, // 1 byte + 31 bytes / 0
    battleType: UInt8, // 1 byte / 32
    defenderHash: Hash256, // 32 bytes / 33
    defenderAddress: XRPAddress, // 20 bytes / 65
    defenderRoll: UInt8, // 1 byte / 85
    attackerHash: Hash256, // 32 bytes / 86
    attackerAddress: XRPAddress, // 20 bytes / 118
    attackerRoll: UInt8 // 1 byte / 138
  ) {
    super()
    this.id = id
    this.battleType = battleType
    this.defenderHash = defenderHash
    this.defenderAddress = defenderAddress
    this.defenderRoll = defenderRoll
    this.attackerHash = attackerHash
    this.attackerAddress = attackerAddress
    this.attackerRoll = attackerRoll
  }

  getMetadata(): Metadata {
    return [
      { field: 'id', type: 'varString', maxStringLength: 31 },
      { field: 'battleType', type: 'uint8' },
      { field: 'defenderHash', type: 'hash256' },
      { field: 'defenderAddress', type: 'xrpAddress' },
      { field: 'defenderRoll', type: 'uint8' },
      { field: 'attackerHash', type: 'hash256' },
      { field: 'attackerAddress', type: 'xrpAddress' },
      { field: 'attackerRoll', type: 'uint8' },
    ]
  }

  toJSON() {
    return {
      id: this.id,
      battleType: this.battleType,
      defenderHash: this.defenderHash,
      defenderAddress: this.defenderAddress,
      defenderRoll: this.defenderRoll,
      attackerHash: this.attackerHash,
      attackerAddress: this.attackerAddress,
      attackerRoll: this.attackerRoll,
    }
  }
}
