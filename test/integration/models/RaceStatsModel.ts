import { RaceHorseModel } from './RaceHorseModel'
import {
  BaseModel,
  Metadata,
  UInt64,
} from '@transia/hooks-toolkit/dist/npm/src/libs/binary-models'

export class RaceStatsModel extends BaseModel {
  lap: UInt64
  horses: RaceHorseModel[]

  // 9 bytes + (64 bytes * # horses)
  constructor(
    lap: UInt64, // 8 bytes / 0
    horses: RaceHorseModel[] // 1 bytes (empty) / 8
  ) {
    super()
    this.lap = lap
    this.horses = horses
  }

  getMetadata(): Metadata {
    return [
      { field: 'lap', type: 'uint64' },
      {
        field: 'horses',
        type: 'varModelArray',
        modelClass: RaceHorseModel,
        maxArrayLength: 10,
      },
    ]
  }

  toJSON() {
    return {
      lap: this.lap,
      horses: this.horses,
    }
  }
}
