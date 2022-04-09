import { Letter } from './alphabet';

export interface IGraph {
    nodes: {
        label: string,
        abbrev: string,
        id: string
    }[],
    edges: {
        from: string,
        to: string,
        id: string
    }[]
}

export interface IRule {
    name: string,
    lhs: IGraph,
    rhs: { probability: number, graph: IGraph }[];
}


export interface GraphProps {
    parent: string,
    rule_index?: number,
    rhs_index?: number,
    graph?: IGraph,
    width?: number,
    height?: number,
    manipulate: boolean,
    letter: Letter

}
