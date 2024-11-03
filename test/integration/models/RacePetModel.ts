import {
  BaseModel,
  VarString,
  UInt8,
  Hash256,
  Metadata,
  UInt64,
} from '@transia/hooks-toolkit/dist/npm/src/libs/binary-models'
import { PetModel } from './PetModel'

export class SimulationModel extends BaseModel {
  id: Hash256
  status: UInt8
  lap: UInt64
  pets: RacePetModel[]

  constructor(id: Hash256, status: UInt8, lap: UInt64, pets: RacePetModel[]) {
    super()
    this.id = id
    this.status = status
    this.lap = lap
    this.pets = pets
  }

  getMetadata(): Metadata {
    return [
      { field: 'id', type: 'hash256' },
      { field: 'status', type: 'uint8' },
      { field: 'lap', type: 'uint64' },
      {
        field: 'pets',
        type: 'varModelArray',
        modelClass: RacePetModel,
        maxArrayLength: 10,
      },
    ]
  }

  toJSON() {
    return {
      id: this.id,
      status: this.status,
      lap: this.lap,
      pets: this.pets,
    }
  }
}

export class RacePetModel extends BaseModel {
  image: string
  position: UInt8
  stall: UInt64
  total: UInt64
  score: UInt64
  percentage: UInt64
  injured: UInt8
  boost: UInt8
  hash: Hash256
  pet: PetModel

  // 82 bytes
  constructor(
    image: VarString, // 8 byte / 0
    position: UInt8, // 8 byte / 0
    stall: UInt64, // 8 byte / 0
    total: UInt64, // 8 byte / 8
    score: UInt64, // 8 byte / 16
    percentage: UInt64, // 8 byte / 24
    injured: UInt8, // 1 byte / 24
    boost: UInt8, // 1 byte / 24
    hash: Hash256, // 33 byte / 25
    pet: PetModel // 90 byte / 32
  ) {
    super()
    this.image = image
    this.position = position
    this.stall = stall
    this.total = total
    this.score = score
    this.percentage = percentage
    this.injured = injured
    this.boost = boost
    this.hash = hash
    this.pet = pet
  }

  getMetadata(): Metadata {
    return [
      { field: 'image', type: 'varString', maxStringLength: 256 },
      { field: 'position', type: 'uint8' },
      { field: 'stall', type: 'uint64' },
      { field: 'total', type: 'uint64' },
      { field: 'score', type: 'uint64' },
      { field: 'percentage', type: 'uint64' },
      { field: 'injured', type: 'uint8' },
      { field: 'boost', type: 'uint8' },
      { field: 'hash', type: 'hash256' },
      { field: 'pet', type: 'model', modelClass: PetModel },
    ]
  }
}
