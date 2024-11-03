import {
  BaseModel,
  Metadata,
  UInt8,
  XFL,
  Hash256,
  UInt64,
} from '@transia/hooks-toolkit/dist/npm/src/libs/binary-models'

export class RaceBetModel extends BaseModel {
  id: UInt64
  state: UInt8
  odd: XFL
  horse: Hash256
  winningPosition: UInt8

  // 50 bytes
  constructor(
    id: UInt64, // 8 byte / 0
    state: UInt8, // 1 byte / 8
    odd: XFL, // 8 byte / 9
    horse: Hash256, // 32 bytes / 17
    winningPosition: UInt8 // 1 byte / 49
  ) {
    super()
    this.id = id
    this.state = state
    this.odd = odd
    this.horse = horse
    this.winningPosition = winningPosition
  }

  getMetadata(): Metadata {
    return [
      { field: 'id', type: 'uint64' },
      { field: 'state', type: 'uint8' },
      { field: 'odd', type: 'xfl' },
      { field: 'horse', type: 'hash256' },
      { field: 'winningPosition', type: 'uint8' },
    ]
  }

  toJSON() {
    return {
      id: this.id,
      state: this.state,
      odd: this.odd,
      horse: this.horse,
      winningPosition: this.winningPosition,
    }
  }
}
