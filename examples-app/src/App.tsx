import React, { useState, useEffect } from 'react';
import './App.css';
import { Vega } from 'react-vega';
import * as Realm from "realm-web";
import Box from '@mui/material/Box';
import InputLabel from '@mui/material/InputLabel';
import Button from '@mui/material/Button';
import MenuItem from '@mui/material/MenuItem';
import FormControl from '@mui/material/FormControl';
import Select from '@mui/material/Select';
import GraphComponent from "./graph-component"
import Slider from '@mui/material/Slider';
import Typography from '@mui/material/Typography';
import TextField from '@mui/material/TextField';
import ReactLoading from 'react-loading';

import dormans from './dormans-grammar.json';

//load graphs from mongo (make sure they get the same set of )
let graphs: any;
let task: any = "Task 1";
export async function grabGraph(setWorkerState: any, worker_state: any) {
    const app: Realm.App = new Realm.App({ id: "application-0-puupx" });
    const uri = "mongodb+srv://admin:admin@graphuser.ihbgs.mongodb.net/myFirstDatabase?retryWrites=true&w=majority";
    const credentials = Realm.Credentials.anonymous();
    try {
        const user: Realm.User = await app.logIn(credentials);
        const mongodb = await user.mongoClient("mongodb-atlas");
        const collGraphs = await mongodb.db("graph_database").collection("graphs");
        const cp = await collGraphs.find();
        const graphs = cp.slice(0, 1000)
        setWorkerState({ ...worker_state, graphs: graphs })
        /* const result = await collGraphs.find({})
* result.limit(1000); */
        /* console.log(result) */
    } catch (err) {
        console.error("Failed to log in", err);
    }

}
export async function connectToDatabase(trained_graphs: any, selected_graphs: any, user_id: any) {
    const app: Realm.App = new Realm.App({ id: "application-0-puupx" });
    const uri = "mongodb+srv://admin:admin@graphuser.ihbgs.mongodb.net/myFirstDatabase?retryWrites=true&w=majority";
    /* console.log("asds"); */
    // Create an anonymous credential
    const credentials = Realm.Credentials.anonymous();
    try {
        // Authenticate the user
        const user: Realm.User = await app.logIn(credentials);
        const mongodb = await user.mongoClient("mongodb-atlas");
        const collGraphs = await mongodb.db("graph_database").collection("graphs");
        var trained_graphs_hashes = [];
        for (let i = 0; i < trained_graphs.length; i++) {
            const result = await collGraphs.updateOne({ hash: trained_graphs[i].hash }, trained_graphs[i], { upsert: true });
            trained_graphs_hashes.push(trained_graphs[i].hash);
        }
        var selected_graphs_hashes = [];
        for (let i = 0; i < selected_graphs.length; i++) {
            selected_graphs_hashes.push(selected_graphs[i].hash);
        }

        const collExamples = await mongodb.db("graph_database").collection("examples");

        /* collExamples.deleteMany({chain_str: ""}) */

        const result = collExamples.insertOne({
            user_id: user_id, selected_graphs: selected_graphs_hashes, trained_graph: trained_graphs_hashes, task: task
        })

        /* console.log(graphs[0]); */
        // exmaple object { user_id, selected_examples, orig_graphs, trained_graphs  }

        // `App.currentUser` updates to match the logged in user
        /* console.log(user.id === app.currentUser.id) */
        return user;
    } catch (err) {
        console.error("Failed to log in", err);
    }

}
function App() {
    const temp: any = null;
    const graphs_s: any = null;
    const trained_graphs: any = null;
    const temp_worker_state: any = { workers: [], graphs: graphs_s, original_graph: null, trained_graph: null, working: false }
    const temp_ui_state: any = { current_task: "Task 1", user_id: "", temp_user_id: "", num_samples: 10, x: "leniency", y: "mission_linearity", mode: 1, selection: null, selection_t: null }
    const [worker_state, setWorkerState] = useState(temp_worker_state);
    const [ui_state, setUIState] = useState(temp_ui_state);
    const [trained_state, setTrainedState] = useState({ trained_graphs: trained_graphs, working: false });
    const signalListnerso = { "brush": handleHover, "select": handleSelect };
    const signalListnersT = { "brush": handleHoverT, "select": handleSelectT };

    function changeSamples(e: any, v: any) {
        setUIState({ ...ui_state, num_samples: v });
    }
    function valuetext(value: any) {
        return `${value}`;
    }
    function returnSpec(graphs: any, trained: boolean) {
        var width = 500;
        var height = 500;
        if (graphs) {
            var single: any = {}
            var signalListners: any = {}
            if (trained) {
                single = {
                    "$schema": "https://vega.github.io/schema/vega-lite/v5.json",
                    "description": "Google's stock price over time.",
                    "data": {
                        "values": graphs
                    },
                    "width": width,
                    "height": height,
                    "mark": { "type": "point", "filled": true, "size": 100 },
                    "params": [{ "name": "select", "select": "single" }],
                    "encoding": {
                        "x": {
                            "field": ui_state.x, "type": "quantitative"
                            , "scale": { "domain": [0, 1] },
                        },
                        "y": {
                            "field": ui_state.y, "type": "quantitative"
                            , "scale": { "domain": [0, 1] },
                        },
                        "color": {
                            "condition": { "param": "select", "type": "ordinal" },
                            "value": "grey"
                        }

                    },

                    "selection":
                    {
                        "select": {
                            "type": "single"
                        }
                    }

                }
                signalListners = signalListnersT
            } else {
                single = {
                    "$schema": "https://vega.github.io/schema/vega-lite/v5.json",
                    "description": "Google's stock price over time.",
                    "data": {
                        "values": graphs
                    },
                    "width": width,
                    "height": height,
                    "mark": { "type": "point", "filled": true, "size": 100 },
                    "params": [{ "name": "select", "select": "single" }],
                    "encoding": {
                        "x": {
                            "field": ui_state.x, "type": "quantitative",
                            "scale": { "domain": [0, 1] }

                        },
                        "y": {
                            "field": ui_state.y, "type": "quantitative",
                            "scale": { "domain": [0, 1] }

                        },
                        "scale": { "domain": [0, 1] },
                        "color": {
                            "condition": { "param": "select", "type": "ordinal" },
                            "value": "grey"
                        }

                    },

                    "selection":
                    {
                        "select": {
                            "type": "single"
                        }
                    }

                }

                signalListners = signalListnerso
            }

            const spec: any = {
                "$schema": "https://vega.github.io/schema/vega-lite/v5.json",
                "description": "Google's stock price over time.",
                "data": {
                    "values": graphs
                },
                "width": width,
                "height": height,
                "mark": { "type": "point", "filled": true, "size": 100 },
                "params": [{ "name": "brush", "select": "interval" }],
                "encoding": {
                    "x": {
                        "field": ui_state.x, "type": "quantitative",
                        "scale": { "domain": [0, 1] }

                    },
                    "y": {
                        "field": ui_state.y, "type": "quantitative",
                        "scale": { "domain": [0, 1] }
                    },
                    "scale": { "domain": [0, 1] },
                    "color": {
                        "condition": { "param": "brush", "type": "ordinal" },
                        "value": "grey"
                    }

                },

                /* "selection": { */
                /* "select": {
            *   "type": "single", */
                /* "encodings": ["x", "y"] */
                /* }
                } */

            }
            if (ui_state.mode === 0) {

                return <Vega spec={spec} signalListeners={signalListners} />
            } else if (ui_state.mode === 1) {

                return <Vega spec={single} signalListeners={signalListners} />
            }
        } else {
            return <div> Not loaded</div>
        }
    }
    useEffect(() => {

        grabGraph(setWorkerState, worker_state);

    }, []);

    useEffect(() => {
        /* console.log("sad"); */
        /* console.log("mad");
         * console.log(state.graphs); */
        /* Module().then(function(mymod: any) {
* });
* console.log(Modu        le); */
        submitUserID('a');
    }, [ui_state]);
    function handleSelectT(name: any, value: any) {
        var trained_graph = trained_state.trained_graphs[value._vgsid_ - 1]
        setWorkerState({ ...worker_state, trained_graph: trained_graph });
    }
    function handleSelect(name: any, value: any) {
        var sk = worker_state.graphs[value._vgsid_ - 1]
        setWorkerState({ ...worker_state, original_graph: sk });
    }

    function handleHover(name: any, value: any) {
        setUIState({ ...ui_state, selection: value });
    }
    function handleHoverT(name: any, value: any) {
        setUIState({ ...ui_state, selection_t: value, selection : null });
    }
    function handleChange(e: any) {

        setUIState({ ...ui_state, mode: e.target.value });

    }

    function handleChangeTask(e: any) {

        task = e.target.value;
        setUIState({ ...ui_state, current_task: e.target.value });

    }
    function handleChangex(e: any) {

        setUIState({ ...ui_state, x: e.target.value });

    }
    function handleChangey(e: any) {

        setUIState({ ...ui_state, y: e.target.value });

    }

    function handleTrain(e: any) {
        if (!ui_state.selection && !ui_state.selection_t) {
            alert("No data points are selected");
        }
        if (ui_state.selection) {
            var value = ui_state.selection;
            const keyss = Object.keys(ui_state.selection);
            var selected_graphs = [];
            const myWorker = new Worker(new URL("generate_n_learnt_chains_worker.js", import.meta.url));

            /* setUIState({ ...ui_state, user_id: ui_state.temp_user_id }); */

            var chain_strs: any = [];
            if(keyss.length > 1){

            for (var i = 0; i < worker_state.graphs.length; i++) {
                if (worker_state.graphs[i][keyss[0]] > value[keyss[0]][0] && worker_state.graphs[i][keyss[0]] < value[keyss[0]][1]) {
                    if (worker_state.graphs[i][keyss[1]] > value[keyss[1]][0] && worker_state.graphs[i][keyss[1]] < value[keyss[1]][1]) {
                        selected_graphs.push(worker_state.graphs[i]);

                        if(worker_state.graphs[i].chain_str === ""){
                            continue;
                        }
                        chain_strs.push(worker_state.graphs[i].chain_str)
                    }
                }
            }

            }else{

            for (var i = 0; i < worker_state.graphs.length; i++) {
                if (worker_state.graphs[i][keyss[0]] > value[keyss[0]][0] && worker_state.graphs[i][keyss[0]] < value[keyss[0]][1]) {
                        selected_graphs.push(worker_state.graphs[i]);

                        if(worker_state.graphs[i].chain_str === ""){
                            continue;
                        }
                        chain_strs.push(worker_state.graphs[i].chain_str)
                    }
            }
            }

            graphs = Array(ui_state.num_samples);
            console.log(selected_graphs);
            for (var i = 0; i < window.navigator.hardwareConcurrency; i++) {
                var num_samples_thread = Math.trunc(ui_state.num_samples / window.navigator.hardwareConcurrency)
                var begin = i * num_samples_thread;
                var end = begin + num_samples_thread;
                if (i === window.navigator.hardwareConcurrency - 1) {
                    end = ui_state.num_samples;
                }
                worker_state.workers[i].postMessage([JSON.stringify(dormans), end - begin, i, begin, end, chain_strs, selected_graphs]);
                setTrainedState({ ...trained_state, working: true })
            }
            /* var graphs: any = []; */
            /* var temp = myWorker.postMessage([chain_strs, JSON.stringify(dormans), ui_state.num_samples]);
* myWorker.onmessage = function(e: any) {
*     graphs = e.data.graphs
*     var selected_graphs = e.data.selected_graphs;
*     setWorkerState({ ...worker_state, trained_graphs: graphs });
* } */
        }
        if(ui_state.selection_t){

            var value = ui_state.selection_t;
            const keyss = Object.keys(ui_state.selection_t);
            var selected_graphs = [];
            const myWorker = new Worker(new URL("generate_n_learnt_chains_worker.js", import.meta.url));

            /* setUIState({ ...ui_state, user_id: ui_state.temp_user_id }); */

            var chain_strs: any = [];
            for (var i = 0; i < trained_state.trained_graphs.length; i++) {
                if (trained_state.trained_graphs[i][keyss[0]] > value[keyss[0]][0] && trained_state.trained_graphs[i][keyss[0]] < value[keyss[0]][1]) {
                    if (trained_state.trained_graphs[i][keyss[1]] > value[keyss[1]][0] && trained_state.trained_graphs[i][keyss[1]] < value[keyss[1]][1]) {
                        selected_graphs.push(trained_state.trained_graphs[i]);

                        if(worker_state.graphs[i].chain_str === ""){
                            continue;
                        }
                        chain_strs.push(trained_state.trained_graphs[i].chain_str)
                    }
                }
            }

            graphs = Array(ui_state.num_samples);
            for (var i = 0; i < window.navigator.hardwareConcurrency; i++) {
                var num_samples_thread = Math.trunc(ui_state.num_samples / window.navigator.hardwareConcurrency)
                var begin = i * num_samples_thread;
                var end = begin + num_samples_thread;
                if (i === window.navigator.hardwareConcurrency - 1) {
                    end = ui_state.num_samples;
                }
                worker_state.workers[i].postMessage([JSON.stringify(dormans), end - begin, i, begin, end, chain_strs, selected_graphs]);
                setTrainedState({ ...trained_state, working: true })
            }
            /* var graphs: any = []; */
            /* var temp = myWorker.postMessage([chain_strs, JSON.stringify(dormans), ui_state.num_samples]);
* myWorker.onmessage = function(e: any) {
*     graphs = e.data.graphs
*     var selected_graphs = e.data.selected_graphs;
*     setWorkerState({ ...worker_state, trained_graphs: graphs });
* } */
        }
    }

    function userIDChange(e: any) {
        setUIState({ ...ui_state, temp_user_id: e.target.value });
    }

    function submitUserID(e: any) {
        //TODO validate the submited id to make sure it is a valid ID
        //might have to grab that cookie
        /* if (!ui_state.temp_user_id || !Number(ui_state.temp_user_id) || ui_state.temp_user_id.length !== 5) { */
            /* console.log(ui_state.temp_user_id.length) */
            /* alert("Invalid user_id");
             * return; */

            /* } */
        /* setUIState({ ...ui_state, user_id: ui_state.temp_user_id }); */

        const workers = worker_state.workers;
        for (var i = 0; i < window.navigator.hardwareConcurrency; i++) {
            const myWorker = new Worker(new URL("./generate_n_worker.js", import.meta.url));
            myWorker.onmessage = function(e: any) {
                var learning = e.data.learning;
                if (graphs) {
                    var l = 0;
                    for (var j = e.data.begin; j < e.data.end; j++) {
                        graphs[j] = e.data.graphs[l]
                        l++;
                    }
                    if (!graphs.includes(undefined)) {
                        if (learning) {
                            setTrainedState({ ...trained_state, trained_graphs: graphs, working: false })
                            /* connectToDatabase(graphs, e.data.selection, ui_state.temp_user_id); */
                        } else {
                            setWorkerState({ ...worker_state, graphs: graphs, working: false })
                        }
                    }
                }
            }
            workers.push(myWorker);
        }
        setWorkerState({ ...worker_state, workers: workers })
        /* const myWorker = new Worker(new URL("./generate_n_worker.js", import.meta.url));
* var graphss: any = [];
* myWorker.onmessage = function(e: any) {
*     console.log("Message recieved");
*     graphss = e.data.graphs
*     setState({ ...state, user_id: state.temp_user_id, graphs: graphss });
* } */

        /* graphs = Array(ui_state.num_samples);
* for (var i = 0; i < window.navigator.hardwareConcurrency; i++) {
*     var num_samples_thread = ui_state.num_samples / window.navigator.hardwareConcurrency
*     var begin = i * num_samples_thread;
*     var end = begin + num_samples_thread;
*     if (i === window.navigator.hardwareConcurrency - 1) {
*         end = ui_state.num_samples;
*     }
*     worker_state.workers[i].postMessage([JSON.stringify(dormans), num_samples_thread, i, begin, end, []]);
*     setWorkerState({ ...worker_state, working: true })
* } */
        /* var temp = myWorker.postMessage([JSON.stringify(dormans), state.num_samples, 0, 0, 0]); */

    }
    function makeUserID() {
        return <div className="box"> <h3> Please enter your Participant ID: </h3>
            <TextField id="user_id" label="Participant ID" onChange={userIDChange} variant="filled" inputProps={{ inputMode: 'numeric', pattern: '[0-9]*' }} />
            <Button className="button" onClick={submitUserID} variant="contained">Next</Button>
        </div>
    }
    function renderLoading() {

        if (worker_state.working || trained_state.working) {
            return <ReactLoading type={"spokes"} color={"1ff"} height={300} width={375} />
        }
    }
    function renderGraph(graph: any, msg: String) {

        if (graph == null) {
            return <div></div>
        }
        return <div> <h2> {msg}</h2>
            <GraphComponent graph={graph} width={600} height={500} manipulate={false} />
        </div>


    }
    function makeApp() {
        /* if (ui_state.user_id) { */
            return <div className="App">
                <div className="sidebar">
                    <Box sx={{ minWidth: 120 }} className="menu">
                        <FormControl fullWidth>
                            <InputLabel id="demo-simple-select-label">Mode</InputLabel>
                            <Select
                                labelId="demo-simple-select-label"
                                id="demo-simple-select"
                                value={ui_state.mode}
                                label="Mode"
                                onChange={handleChange}
                            >
                                <MenuItem value={0}>Train</MenuItem>
                                <MenuItem value={1}>Analyze</MenuItem>
                            </Select>
                        </FormControl>
                    </Box>
                    <Box sx={{ minWidth: 120 }} className="menu">
                        <FormControl fullWidth>
                            <InputLabel id="demo-simple-select-label">X-Axis</InputLabel>
                            <Select
                                labelId="demo-simple-select-label"
                                id="demo-simple-select"
                                value={ui_state.x}
                                label="Mode"
                                onChange={handleChangex}
                            >
                                <MenuItem value={"leniency"}>Leniency</MenuItem>
                                <MenuItem value={"mission_linearity"}>Mission Linearity</MenuItem>
                                <MenuItem value={"map_linearity"}>Map Linearity</MenuItem>
                                <MenuItem value={"path_redundancy"}>Path Redundancy</MenuItem>
                            </Select>
                        </FormControl>
                    </Box>
                    <Box className="menu" sx={{ minWidth: 120 }}>
                        <FormControl fullWidth>
                            <InputLabel id="demo-simple-select-label">Y-Axis</InputLabel>
                            <Select
                                labelId="demo-simple-select-label"
                                id="demo-simple-select"
                                value={ui_state.y}
                                label="Mode"
                                onChange={handleChangey}
                            >
                                <MenuItem value={"leniency"}>Leniency</MenuItem>
                                <MenuItem value={"mission_linearity"}>Mission Linearity</MenuItem>
                                <MenuItem value={"map_linearity"}>Map Linearity</MenuItem>
                                <MenuItem value={"path_redundancy"}>Path Redundancy</MenuItem>
                            </Select>
                        </FormControl>
                    </Box>
                    {/* <Box className="menu" sx={{ minWidth: 120 }}>
                        <FormControl fullWidth>
                        <InputLabel id="demo-simple-select-label">Current Task</InputLabel>
                        <Select
                        labelId="demo-simple-select-label"
                        id="demo-simple-select"
                        value={ui_state.current_task}
                        label="Current Task"
                        onChange={handleChangeTask}
                        >
                        <MenuItem value={"Task 1"}>Task 1</MenuItem>
                        <MenuItem value={"Task 2"}>Task 2</MenuItem>
                        <MenuItem value={"Task 3"}>Task 3</MenuItem>
                        <MenuItem value={"Task 4"}>Task 4</MenuItem>
                        <MenuItem value={"Task 5"}>Task 5</MenuItem>
                        <MenuItem value={"Task 6"}>Task 6</MenuItem>
                        <MenuItem value={"Task 7"}>Task 7</MenuItem>
                        </Select>
                        </FormControl>
                        </Box> */}
                    <Slider style={{ width: "90%" }}
                        aria-label="Temperature"
                        defaultValue={ui_state.num_samples}
                        getAriaValueText={valuetext}
                        valueLabelDisplay="auto"
                        onChangeCommitted={changeSamples}
                        step={10}
                        marks
                        min={10}
                        max={100}
                    />
                    <Typography gutterBottom>Number of samples </Typography>
                    <Button className="button" onClick={handleTrain} variant="contained">Train</Button>
                </div>
                <div className="charts">
                    {renderLoading()}
                    {returnSpec(worker_state.graphs, false)}
                    {returnSpec(trained_state.trained_graphs, true)}
                </div>
                <div className="graphs">
                    {renderGraph(worker_state.original_graph, "Graph from Original Grammar")}
                    <div>
                        {renderGraph(worker_state.trained_graph, "Graph from Trained Grammar")}
                    </div>
                </div>
            </div>
        /* } else {
         *     return <div className="container">
         *         {makeUserID()}
         *     </div>
         * } */
    }
    return <div>
        {makeApp()}
    </div>

}

export default App;
