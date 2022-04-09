export interface Letter {
    label: string,
    abbrev: string,
    terminality: boolean
}
export interface IAlphabet {
    _id: string,
    name: string,
    letters: Array<Letter>

}
