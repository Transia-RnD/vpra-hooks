import {
  BaseModel,
  Metadata,
  UInt8,
  UInt64,
  XFL,
  Hash256,
  VarString,
} from '@transia/hooks-toolkit/dist/npm/src/libs/binary-models'

export class PetModel extends BaseModel {
  name: VarString
  id: UInt64
  gender: UInt8
  age: UInt8
  breed: UInt8
  size: UInt8
  body: UInt8
  hooves: UInt8
  speed: UInt8
  stamina: UInt8
  temperament: UInt8
  training: UInt8
  health: UInt8
  lifespan: UInt8
  affinity: UInt8
  morale: UInt64
  isBreedable: UInt8
  breedPrice: XFL
  tokenId: Hash256
  wins: UInt64
  total: XFL

  // 100 bytes
  constructor(
    id: UInt64, // 8 bytes / 0
    name: VarString, // 1 byte + 13 bytes / 8
    gender: UInt8, // 1 byte / 22
    age: UInt8, // 1 byte / 23
    breed: UInt8, // 1 byte / 24
    size: UInt8, // 1 byte / 25
    body: UInt8, // 1 byte / 26
    hooves: UInt8, // 1 byte / 27
    speed: UInt8, // 1 byte / 28
    stamina: UInt8, // 1 byte / 29
    temperament: UInt8, // 1 byte / 30
    training: UInt8, // 1 byte / 31
    health: UInt8, // 1 byte / 32
    lifespan: UInt8, // 1 byte / 33
    affinity: UInt8, // 1 byte / 34
    morale: UInt64, // 8 byte / 35
    isBreedable: UInt8, // 1 byte / 43
    breedPrice: XFL, // 8 bytes / 44
    tokenId: Hash256, // 32 bytes / 52
    wins: UInt64, // 8 bytes / 84
    total: XFL // 8 bytes / 92
  ) {
    super()
    this.id = id
    this.name = name
    this.gender = gender
    this.age = age
    this.breed = breed
    this.size = size
    this.body = body
    this.hooves = hooves
    this.speed = speed
    this.stamina = stamina
    this.temperament = temperament
    this.training = training
    this.health = health
    this.lifespan = lifespan
    this.affinity = affinity
    this.morale = morale
    this.isBreedable = isBreedable
    this.breedPrice = breedPrice
    this.tokenId = tokenId
    this.wins = wins
    this.total = total
  }

  getMetadata(): Metadata {
    return [
      { field: 'id', type: 'uint64' },
      { field: 'name', type: 'varString', maxStringLength: 13 },
      { field: 'gender', type: 'uint8' },
      { field: 'age', type: 'uint8' },
      { field: 'breed', type: 'uint8' },
      { field: 'size', type: 'uint8' },
      { field: 'body', type: 'uint8' },
      { field: 'hooves', type: 'uint8' },
      { field: 'speed', type: 'uint8' },
      { field: 'stamina', type: 'uint8' },
      { field: 'temperament', type: 'uint8' },
      { field: 'training', type: 'uint8' },
      { field: 'health', type: 'uint8' },
      { field: 'lifespan', type: 'uint8' },
      { field: 'affinity', type: 'uint8' },
      { field: 'morale', type: 'uint64' },
      { field: 'isBreedable', type: 'uint8' },
      { field: 'breedPrice', type: 'xfl' },
      { field: 'tokenId', type: 'hash256' },
      { field: 'wins', type: 'xfl' },
      { field: 'total', type: 'uint64' },
    ]
  }
}
