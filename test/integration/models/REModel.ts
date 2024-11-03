import {
  BaseModel,
  Metadata,
  Hash256,
  UInt8,
} from '@transia/hooks-toolkit/dist/npm/src/libs/binary-models'

export class REHashModel extends BaseModel {
  hash: Hash256

  // 32 bytes
  constructor(
    hash: Hash256 // 32 bytes / 0
  ) {
    super()
    this.hash = hash
  }

  getMetadata(): Metadata {
    return [{ field: 'hash', type: 'hash256' }]
  }

  toJSON() {
    return {
      hash: this.hash,
    }
  }
}

export class REBetModel extends BaseModel {
  hash: Hash256
  position: UInt8
  slips: REHashModel[]

  // 354 bytes
  constructor(
    hash: Hash256, // 32 bytes / 0
    position: UInt8, // 1 bytes / 32
    slips: REHashModel[] // 1 bytes + (10 * 32) / 33
  ) {
    super()
    this.hash = hash
    this.position = position
    this.slips = slips
  }

  getMetadata(): Metadata {
    return [
      { field: 'hash', type: 'hash256' },
      { field: 'position', type: 'uint8' },
      {
        field: 'slips',
        type: 'varModelArray',
        modelClass: REHashModel,
        maxArrayLength: 10,
      },
    ]
  }

  toJSON() {
    return {
      hash: this.hash,
      position: this.position,
      slips: this.slips,
    }
  }
}

export class REBetsModel extends BaseModel {
  bets: REBetModel[]

  // 3054 bytes
  constructor(
    bets: REBetModel[] // 354 * 10 / 0
  ) {
    super()
    this.bets = bets
  }

  getMetadata(): Metadata {
    return [
      {
        field: 'bets',
        type: 'varModelArray',
        modelClass: REBetModel,
        maxArrayLength: 10,
      },
    ]
  }

  toJSON() {
    return {
      bets: this.bets,
    }
  }
}

export class REModel extends BaseModel {
  horses: REHashModel[]

  // 96 bytes
  constructor(
    horses: REHashModel[] // 32 * 3 / 0
  ) {
    super()
    this.horses = horses
  }

  getMetadata(): Metadata {
    return [
      {
        field: 'horses',
        type: 'varModelArray',
        modelClass: REHashModel,
        maxArrayLength: 3,
      },
    ]
  }

  toJSON() {
    return {
      horses: this.horses,
    }
  }
}
