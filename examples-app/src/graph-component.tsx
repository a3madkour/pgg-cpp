import React, { createRef, useEffect, useContext } from 'react';
import { Network } from "vis-network/standalone/esm/vis-network"
import { Letter } from './util/alphabet';
import { IGraph } from './util/graph';
import colors from './colors.json';

interface Props {
    graph: IGraph,
    width?: number,
    height?: number,
    updateParent?: () => void,
    manipulate: boolean
    letter?: Letter
}

export default function GraphComponent(props: Props) {

    const myRef = createRef<HTMLDivElement>();
    const largeHeight = props.height == null ? 100 : props.height
    const largeWidth = props.width == null ? 100 : props.width


    var network = {}

    const [state, setState] = React.useState({});
    const handleAddNode = (nodeData: any, callback: any) => {

        if (!props.letter) {
            console.log("this is where I should alter the user that no icon is selected")
            return
        }
        var color = "#000000"
        var svg =
            '<svg xmlns="http://www.w3.org/2000/svg" width="75" height="75">' +
            '<circle cx="37.5" cy="37.5" r="35" fill="' + color + '"/>' +
            '<text x="50%" y="50%" text-anchor="middle" fill="white" font-size="x-large" font-family="Arial" dy=".3em">' + props.letter.abbrev + '</text>' +
            'Sorry, your browser does not support inline SVG.' +
            "</svg>";

        var url = "data:image/svg+xml;charset=utf-8," + encodeURIComponent(svg);
        /* console.log(svg);
         * console.log(url); */

        nodeData.label = props.letter.label;
        nodeData.shape = "circularImage";
        nodeData.abbrev = props.letter.abbrev
        nodeData.image = url;
        callback(nodeData);
        props.graph.nodes.push({ label: nodeData.label, abbrev: nodeData.abbrev, id: nodeData.id });
        if (props.updateParent) {
            props.updateParent();
        }
    }

    const handleAddEdge = (edgeData: any, callback: any) => {
        callback(edgeData);
        props.graph.edges.push(edgeData)
        if (props.updateParent) {
            props.updateParent();
        }
    }

    const handleDeleteNode = (nodeData: any, callback: any) => {
        callback(nodeData);
        for (var i = 0; i < nodeData.nodes.length; i++) {
            for (var j = 0; j < props.graph.nodes.length; j++) {
                if (nodeData.nodes == props.graph.nodes[j].id) {
                    props.graph.nodes.splice(j, 1);
                }
            }
        }
        for (var i = 0; i < nodeData.edges.length; i++) {
            for (var j = 0; j < props.graph.edges.length; j++) {
                if (nodeData.edges == props.graph.edges[j].id) {
                    props.graph.edges.splice(j, 1);
                }
            }
        }
        if (props.updateParent) {
            props.updateParent();
        }
    }
    const handleDeleteEdge = (edgeData: any, callback: any) => {
        callback(edgeData);
        for (var i = 0; i < edgeData.edges.length; i++) {
            for (var j = 0; j < props.graph.edges.length; j++) {
                if (edgeData.edges == props.graph.edges[j].id) {
                    props.graph.edges.splice(j, 1);
                }
            }
        }
        if (props.updateParent) {
            props.updateParent();
        }
    }
    var options = {
        interaction: { hover: true },
        edges: {
            arrows: { to: true }
        },
        manipulation: {
            enabled: props.manipulate,
            addNode: handleAddNode,
            addEdge: handleAddEdge,
            deleteNode: handleDeleteNode,
            deleteEdge: handleDeleteEdge
        },

    }

    // create an array with edges

    /* var data = {
     *     nodes: props.graph.nodes,
     *     edges: props.graph.edges,
     * }; */

    var processGraph = (graph: IGraph) => {

        var nodes: Array<any> = []
        var edges: Array<any> = []
        var data = { nodes: nodes, edges: edges }
        var color_map: { [key: string]: any } = colors;

        for (var i = 0; i < graph.nodes.length; i++) {
            var node: any = Object.assign({}, graph.nodes[i]);
            /* var color = "#aeaeae" */
            /* console.log(colors["e"]); */
            var ab = graph.nodes[i].abbrev;
            var color = color_map[ab];
            /* var color = "#999211" */
            var svg =
                '<svg xmlns="http://www.w3.org/2000/svg" width="75" height="75">' +
                '<circle cx="37.5" cy="37.5" r="35" fill="' + color + '" />' +
                '<text x="50%" y="50%" text-anchor="middle" fill="white" font-size="x-large" font-family="Arial" dy=".3em">' + graph.nodes[i].abbrev + '</text>' +
                'Sorry, your browser does not support inline SVG.' +
                "</svg>";

            var url = "data:image/svg+xml;charset=utf-8," + encodeURIComponent(svg);

            node.id = node.inder
            node.shape = "circularImage";
            node.image = url;
            data.nodes.push(node)
            /* graph.nodes[i].delete(); */
        }
        data.edges = graph.edges

        return data
    }

    useEffect(() => {

        var divElement = myRef.current
        if (divElement == null) {
            return;
        }

        processGraph(props.graph)

        var data = processGraph(props.graph)
        var network = new Network(divElement, data, options)
        setState(network)

        if (props.updateParent) {
            props.updateParent();
        }

    }, [props.graph]);
    useEffect(() => {
        //I hate myself
        if (state instanceof Network) {
            state.setOptions(options);
        }
    }, [props.letter]);

    return (
        <div style= {{height: props.height, minWidth: props.width+"px", maxWidth: props.width+"px"}} className="graphWindow" ref={myRef} >
        </div>

    )

}
