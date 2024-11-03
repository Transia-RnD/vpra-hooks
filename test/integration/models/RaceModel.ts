import {
  BaseModel,
  Metadata,
  UInt8,
  UInt32,
  UInt64,
  XFL,
  VarString,
} from '@transia/hooks-toolkit/dist/npm/src/libs/binary-models'

export class RaceModel extends BaseModel {
  id: UInt64
  name: VarString
  status: UInt8 // 0 pending 1 started 2 ended 3 cancelled
  weightClass: UInt8
  weather: UInt8
  soil: UInt8
  stalls: UInt8
  length: UInt32
  start: UInt32
  entryFee: XFL
  poolFee: XFL
  trackFee: XFL

  // 77 bytes
  constructor(
    id: UInt64, // 8 bytes // 0
    name: VarString, // 1 byte + 31 bytes / 8
    status: UInt8, // 1 bytes // 40
    weightClass: UInt8, // 1 bytes / 41
    weather: UInt8, // 1 bytes / 42
    soil: UInt8, // 1 bytes / 43
    stalls: UInt8, // 1 bytes / 44
    length: UInt32, // 4 bytes / 45
    start: UInt32, // 4 bytes / 49
    entryFee: XFL, // 8 bytes / 53
    poolFee: XFL, // 8 bytes / 61
    trackFee: XFL // 8 bytes / 69
  ) {
    super()
    this.id = id
    this.name = name
    this.status = status
    this.weightClass = weightClass
    this.weather = weather
    this.soil = soil
    this.stalls = stalls
    this.length = length
    this.start = start
    this.entryFee = entryFee
    this.poolFee = poolFee
    this.trackFee = trackFee
  }

  getMetadata(): Metadata {
    return [
      { field: 'id', type: 'uint64' },
      { field: 'name', type: 'varString', maxStringLength: 31 },
      { field: 'status', type: 'uint8' },
      { field: 'weightClass', type: 'uint8' },
      { field: 'weather', type: 'uint8' },
      { field: 'soil', type: 'uint8' },
      { field: 'stalls', type: 'uint8' },
      { field: 'length', type: 'uint32' },
      { field: 'start', type: 'uint32' },
      { field: 'entryFee', type: 'xfl' },
      { field: 'poolFee', type: 'xfl' },
      { field: 'trackFee', type: 'xfl' },
    ]
  }

  toJSON() {
    return {
      id: this.id,
      name: this.name,
      status: this.status,
      weightClass: this.weightClass,
      weather: this.weather,
      soil: this.soil,
      stalls: this.stalls,
      length: this.length,
      start: this.start,
      entryFee: this.entryFee,
      poolFee: this.poolFee,
      trackFee: this.trackFee,
    }
  }
}
