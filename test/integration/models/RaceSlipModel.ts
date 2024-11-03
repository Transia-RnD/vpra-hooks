import {
  BaseModel,
  Metadata,
  UInt64,
  UInt8,
  XFL,
  XRPAddress,
} from '@transia/hooks-toolkit/dist/npm/src/libs/binary-models'

export class RaceSlipModel extends BaseModel {
  id: UInt64 // 1 or 2
  position: UInt8 // 1 or 2
  value: XFL
  win: XFL
  toSeal: XFL
  executed: UInt8
  refunded: UInt8
  account: XRPAddress

  // 55 bytes
  constructor(
    id: UInt64, // 8 byte / 0
    position: UInt8, // 1 byte / 8
    value: XFL, // 8 byte / 9
    win: XFL, // 8 byte / 17
    toSeal: XFL, // 8 byte / 25
    executed: UInt8, // 1 byte / 33
    refunded: UInt8, // 1 byte / 34
    account: XRPAddress // 20 byte / 35
  ) {
    super()
    this.id = id
    this.position = position
    this.value = value
    this.win = win
    this.toSeal = toSeal
    this.executed = executed
    this.refunded = refunded
    this.account = account
  }

  getMetadata(): Metadata {
    return [
      { field: 'id', type: 'uint64' },
      { field: 'position', type: 'uint8' },
      { field: 'value', type: 'xfl' },
      { field: 'win', type: 'xfl' },
      { field: 'toSeal', type: 'xfl' },
      { field: 'executed', type: 'uint8' },
      { field: 'refunded', type: 'uint8' },
      { field: 'account', type: 'xrpAddress' },
    ]
  }

  toJSON() {
    return {
      id: this.id,
      position: this.position,
      value: this.value,
      win: this.win,
      toSeal: this.toSeal,
      executed: this.executed,
      refunded: this.refunded,
      account: this.account,
    }
  }
}
