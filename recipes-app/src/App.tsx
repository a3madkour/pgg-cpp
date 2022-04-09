import React, { useState, useEffect } from 'react';
import './App.css';
import { Vega } from 'react-vega';
import * as Realm from "realm-web";
import Box from '@mui/material/Box';
import InputLabel from '@mui/material/InputLabel';
import Checkbox from '@mui/material/Checkbox';
import TextField from '@mui/material/TextField';
import Button from '@mui/material/Button';
import MenuItem from '@mui/material/MenuItem';
import FormControl from '@mui/material/FormControl';
import AddIcon from '@mui/icons-material/Add';
import FormControlLabel from '@mui/material/FormControlLabel';
import Select from '@mui/material/Select';
import GraphComponent from "./graph-component"
import RuleAppComponent from "./rule-app-component"
import Module from "./quick_example.js";
import Fab from '@mui/material/Fab';
import dormans from './dormans-grammar.json';
import Slider from '@mui/material/Slider';
import Typography from '@mui/material/Typography';
import ReactLoading from 'react-loading';
let graphs: any;
let task: any = "Task 1";
export async function connectToDatabase(graphs: any, recipe: any, user_id: any) {
    const app: Realm.App = new Realm.App({ id: "application-0-puupx" });
    const uri = "mongodb+srv://admin:admin@graphuser.ihbgs.mongodb.net/myFirstDatabase?retryWrites=true&w=majority";
    /* console.log("asds"); */
    // Create an anonymous credential
    console.log(task);
    const credentials = Realm.Credentials.anonymous();
    try {
        // Authenticate the user
        const user: Realm.User = await app.logIn(credentials);
        const mongodb = await user.mongoClient("mongodb-atlas");
        const collGraphs = await mongodb.db("graph_database").collection("graphs");
        var original_graphs_hashes = [];
        for (let i = 0; i < graphs.length; i++) {
            const result = await collGraphs.updateOne({ hash: graphs[i].hash }, graphs[i], { upsert: true });
            original_graphs_hashes.push(graphs[i].hash);
        }


        const collRecipes = await mongodb.db("graph_database").collection("recipes");
        const result = collRecipes.insertOne({
            user_id: user_id, original_graphs: original_graphs_hashes, recipe: recipe, task: task
        })
        return user;
    } catch (err) {
        console.error("Failed to log in", err);
    }
}

function App() {
    const temp: any = null;
    const temp_arr: any = [];
    const default_recipe_entry = { rule: { name: "Start rule" }, min: 1, max: 1, selected_rule: 0, applicable_rules: ["Start rule"] }
    const [worker_state, setWorkerState] = useState({ workers: temp_arr, graphs: graphs, original_graph: null, trained_graph: null, working: false });
    const [ui_state, setUIState] = useState({ current_task: "Task 1", module: temp, user_id: "", temp_user_id: "", num_samples: 10, graphs: graphs, x: "leniency", y: "mission_linearity", mode: 0, rule_apps: [], recipe: [default_recipe_entry], grammar_rule_names: [], cont: true });

    function makeGraph(c_graph: any) {
        var nodes: any = [];
        var edges = [];
        var map: any = {};

        for (var h = 0; h < c_graph.nodes.size(); h++) {
            var node = c_graph.nodes.get(h);
            var node_cp = {
                inder: h, id: node.id, abbrev: node.abbrev, label: node.label
            }
            map[node.id] = h;
            nodes.push(node_cp);
            node.delete();

        }
        c_graph.nodes.delete();
        for (var es = 0; es < c_graph.edges.size(); es++) {
            var temp_ed = c_graph.edges.get(es);
            var edge = { "from": map[temp_ed.from], "to": map[temp_ed.to] }
            edges.push(edge);
            temp_ed.delete();

        }
        c_graph.edges.delete();

        var k: any = { "nodes": nodes, "edges": edges }
        k.chain_str = c_graph["chain_str"]
        k.map_linearity = c_graph["map_linearity"];
        k.leniency = c_graph["leniency"];
        k.mission_linearity = c_graph["mission_linearity"];
        k.path_redundancy = c_graph["path_redundancy"];
        k.hash = c_graph["hash"];
        return k;
    }
    function returnSpec(graphs: any) {

        if (graphs && ui_state.mode === 1) {
            var single: any = {}
            single = {
                "$schema": "https://vega.github.io/schema/vega-lite/v5.json",
                "description": "Google's stock price over time.",
                "data": {
                    "values": graphs
                },
                "width": 400,
                "height": 400,
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

            return <Vega spec={single} signalListeners={signalListners} />

        }
    }
    useEffect(() => {
        var grammar_rule_names: any = [];
        for (var kl = 0; kl < dormans.rules.length; kl++) {
            grammar_rule_names.push(dormans.rules[kl].name)

        }
        const mod = Module().then((prop: any) => {
            var rule_names = [];
            var vec: any = new prop.recipe();
            var temp = prop.apply_recipe(JSON.stringify(dormans), vec);

            var app_rules = temp.app_rules;
            for (var j = 0; j < app_rules.size(); j++) {
                var rule_name = app_rules.get(j);
                rule_names.push(rule_name);
            }

            app_rules.delete();
            vec.delete();
            var graph = makeGraph(temp);
            temp.delete();
            var rule_app = { graph: graph, rule_names: rule_names, selected_rule: 0 }
            var rule_apps: any = ui_state.rule_apps;
            rule_apps.push(rule_app);

            setUIState({ ...ui_state, module: prop, grammar_rule_names: grammar_rule_names });
        });

    }, []);


    useEffect(() => {
        /* console.log("sad"); */
        submitUserID("asd");
    }, [ui_state]);
    function handleSelect(name: any, value: any) {
        var sk = worker_state.graphs[value._vgsid_ - 1]
        /* console.log("original graph: ")
    * console.log(original_graph) */
        /* console.log("change has happened") */
        setWorkerState({ ...worker_state, original_graph: sk });
    }
    const signalListners = { "select": handleSelect };


    function handleChangeR(e: any) {

        /* console.log("rule index: ", e.target); */
        const index = parseInt(e.target.name)
        var recipe = ui_state.recipe
        recipe[index].selected_rule = e.target.value;
        recipe[index].rule.name = ui_state.grammar_rule_names[e.target.value];
        setUIState({ ...ui_state, recipe: recipe });

    }


    function handleChange(e: any) {

        setUIState({ ...ui_state, mode: e.target.value });

    }
    function handleChangex(e: any) {

        setUIState({ ...ui_state, x: e.target.value });

    }
    function handleChangeTask(e: any) {

        task = e.target.value;
        setUIState({ ...ui_state, current_task: e.target.value });

    }
    function handleChangey(e: any) {

        setUIState({ ...ui_state, y: e.target.value });

    }

    function handleDelete(e: any) {
        var recipe = ui_state.recipe;
        const index = e.target.value;
        recipe = recipe.splice(0, index);
        setUIState({ ...ui_state, recipe: recipe });
    }
    function handleApply(e: any) {
        var minMax = true;
        for (var i = 0; i < ui_state.recipe.length; i++) {
            if (ui_state.recipe[i].min > ui_state.recipe[i].max) {
                minMax = false;
                alert("Min is bigger than max at index: " + i);
            }
        }
        var grphs = [];
        if (minMax) {
            /* console.log(ui_state.num_samples); */

            graphs = Array(ui_state.num_samples);
            /* console.log(graphs) */
            for (var i = 0; i < window.navigator.hardwareConcurrency; i++) {
                var num_samples_thread = Math.trunc(ui_state.num_samples / window.navigator.hardwareConcurrency)
                var begin = i * num_samples_thread;
                var end = begin + num_samples_thread;
                if (i === window.navigator.hardwareConcurrency - 1) {
                    end = ui_state.num_samples;
                }
                worker_state.workers[i].postMessage([JSON.stringify(dormans), end- begin , i, begin, end, ui_state.cont, ui_state.recipe]);
            }
            setWorkerState({ ...worker_state, working: true })
        }
    }
    function renderGraph(graph: any, height: any, width: any) {

        if (graph == null || ui_state.mode === 0) {
            return <div></div>

        }
        return <GraphComponent graph={graph} height={height} width={width} manipulate={false} />


    }
    function makeRuleNames(recipe_index: any) {
        var kl = [];
        for (var i = 0; i < ui_state.recipe[recipe_index].applicable_rules.length; i++) {
            kl.push(<MenuItem value={i} > {ui_state.recipe[recipe_index].applicable_rules[i]} </MenuItem>);
        }
        return kl;
    }

    const deleteButton = (i: number) => {
        if (i > 0) {

            return <Button onClick={handleDelete} value={i} variant="contained" color="error">Delete</Button>
        }
    }

    const handleMinChange = (e: any) => {
        const index = parseInt(e.target.id)
        var recipe = ui_state.recipe;
        recipe[index].min = parseInt(e.target.value);
    }

    const handleMaxChange = (e: any) => {
        const index = parseInt(e.target.id)
        var recipe = ui_state.recipe;
        recipe[index].max = parseInt(e.target.value);
    }
    function writeRecipeEntries() {
        var recipe_comp = [];
        for (var i = 0; i < ui_state.recipe.length; i++) {
            /* console.log(state.recipe[i]); */
            recipe_comp.push(
                <div className="recipeEntry">
                    <div style={{ flex: 3 }}>
                        <FormControl fullWidth>
                            <InputLabel id="demo-simple-select-label">Mode</InputLabel>
                            <Select
                                labelId="demo-simple-select-label"
                                id="demo-simple-select"
                                value={ui_state.recipe[i].selected_rule}
                                name={"" + i}
                                label="Rule"
                                onChange={(e: any) => { handleChangeR(e) }}
                            >
                                {makeRuleNames(i)}
                            </Select>
                        </FormControl>
                    </div>
                    <TextField id={i + ""} style={{ flex: 1 }} defaultValue={ui_state.recipe[i].min} onChange={handleMinChange} label="min" inputProps={{ inputMode: 'numeric', pattern: '[0-9]*' }} />
                    <TextField id={i + ""} style={{ flex: 1 }} defaultValue={ui_state.recipe[i].max} onChange={handleMaxChange} label="max" inputProps={{ inputMode: 'numeric', pattern: '[0-9]*' }} />
                    {deleteButton(i)}
                </div>
            );
        }
        return recipe_comp
    }

    const handleAddRule = () => {
        var recipe = ui_state.recipe;
        const current_index = recipe.length - 1;
        var vec: any = new ui_state.module.recipe();
        var minMax = true;
        for (var i = 0; i < recipe.length; i++) {
            var re: any = new ui_state.module.RecipeEntry();
            re.rule_name = ui_state.recipe[i].applicable_rules[ui_state.recipe[i].selected_rule];
            /* console.log("rule_name: ", re.rule_name);
* console.log("selected_rule: ", ui_state.recipe[i].selected_rule); */
            re.min = ui_state.recipe[i].min;
            re.max = ui_state.recipe[i].max;
            re.index = i;
            vec.push_back(re);
            /* re.delete(); */
            if (ui_state.recipe[i].min > ui_state.recipe[i].max) {
                minMax = false;
                alert("Min is bigger than max at index: " + i);
            }
        }

        if (minMax) {
            var temp = ui_state.module.apply_recipe(JSON.stringify(dormans), vec);

            var app_rules = temp.app_rules;
            var rule_names: Array<string> = [];
            for (var j = 0; j < app_rules.size(); j++) {
                var rule_name = app_rules.get(j);
                rule_names.push(rule_name);
            }
            app_rules.delete();
            rule_names = rule_names.filter((element, index) => {
                return rule_names.indexOf(element) === index;
            });
            const recipe_entry = { rule: { name: ui_state.grammar_rule_names[ui_state.recipe[current_index].selected_rule] }, min: ui_state.recipe[current_index].min, max: ui_state.recipe[current_index].max, selected_rule: ui_state.recipe[current_index].selected_rule, applicable_rules: rule_names }
            /* console.log(recipe_entry) */

            recipe.push(recipe_entry);
            setUIState({ ...ui_state, recipe: recipe });
        }
        for (var i = 0; i < current_index + 1; i++) {
            var res: any = vec.get(i);
            res.delete();
        }
        vec.delete();
    }


    function writeButton() {

        if (ui_state.mode === 1) {
            return <div className="addIcon">
                <Fab color="primary" onClick={handleAddRule}>
                    <AddIcon />
                </Fab>
            </div>
        }
    }
    function writeRecipe() {
        if (ui_state.mode === 1) {
            return <div style={{ width: "100%" }}>
                {writeRecipeEntries()}
            </div >
        }


    }
    function handleRuleChange(e: any) { }
    function handleApplyRule(e: any) {
        /* console.log(e); */
        /* var rule_apps = state.rule_apps; */
        /* rule_apps.push("whoops") */
        /* setState({ graphs: graphs, x: state.x, y: state.y, mode: state.mode, original_graph: null, trained_graph: null, rule_apps: rule_apps, recipe: state.recipe }); */

    }
    function applyStep(index: any, selected_index: any) {

        var rule_apps: any = ui_state.rule_apps;
        rule_apps[index].selected_rule = selected_index;
        rule_apps.splice(index + 1);
        var vec: any = new ui_state.module.recipe();
        /* var rec */
        for (var i = 0; i < index + 1; i++) {
            /* console.log(i); */
            var re: any = new ui_state.module.RecipeEntry();
            re.rule_name = rule_apps[i].rule_names[rule_apps[i].selected_rule];
            re.min = 1;
            re.max = 1;
            re.index = i;
            vec.push_back(re);
            /* re.delete(); */
        }

        var temp = ui_state.module.apply_recipe(JSON.stringify(dormans), vec);
        for (var i = 0; i < index + 1; i++) {
            var res: any = vec.get(i);
            res.delete();
        }
        vec.delete();
        var graph = makeGraph(temp);
        var app_rules = temp.app_rules;
        var rule_names: any = [];
        for (var j = 0; j < app_rules.size(); j++) {
            var rule_name = app_rules.get(j);
            rule_names.push(rule_name);
        }
        rule_names = rule_names.filter((element: any, index: any) => {
            return rule_names.indexOf(element) === index;
        });
        var rule_app = { graph: graph, rule_names: rule_names, selected_rule: 0 }
        temp.delete();
        rule_apps.push(rule_app);
        setUIState({ ...ui_state, rule_apps: rule_apps });
    }

    function makeRuleApps() {
        var rule_apps: any = [];
        for (var i = 0; i < ui_state.rule_apps.length; i++) {

            rule_apps.push(
                <RuleAppComponent index={i} ruleApp={ui_state.rule_apps[i]} callback={applyStep} />
            )

        }
        return rule_apps;

    }

    function stepByStep() {
        if (ui_state.mode === 0) {
            return <div className="steps">
                {makeRuleApps()}
            </div>
        }
        return
    }
    function changeSamples(e: any, v: any) {
        /* console.log(v); */
        setUIState({ ...ui_state, num_samples: v });
    }
    function valuetext(value: any) {
        return `${value}`;
    }

    function numSamples() {
        if (ui_state.mode === 1) {
            return <Slider style={{ width: "90%" }}
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
        }

    }
    function applyRecipeButton() {
        if (ui_state.mode === 1) {
            return <div><Typography gutterBottom>Number of samples </Typography>
                <Button onClick={handleApply} variant="contained">Apply Recipe</Button>
            </div>
        }
    }

    function userIDChange(e: any) {
        setUIState({ ...ui_state, temp_user_id: e.target.value });
    }
    function submitUserID(e: any) {
        /* if (!ui_state.temp_user_id || !Number(ui_state.temp_user_id) || ui_state.temp_user_id.length !== 5) {
         *     alert("Invalid user_id");
         *     return;
         * }
         * setUIState({ ...ui_state, user_id: ui_state.temp_user_id }); */

        const workers = worker_state.workers;
        for (var i = 0; i < window.navigator.hardwareConcurrency; i++) {
            const myWorker = new Worker(new URL("./apply_recipe_n_worker.js", import.meta.url));
            myWorker.onmessage = function(e: any) {
                var learning = e.data.learning;
                /* console.log(graphs) */
                if (graphs) {
                    var l = 0;
                    for (var j = e.data.begin; j < e.data.end; j++) {
                        graphs[j] = e.data.graphs[l]
                        l++;
                    }
                    if (!graphs.includes(undefined)) {
                        setWorkerState({ ...worker_state, graphs: graphs, working: false })
                        /* connectToDatabase(graphs, ui_state.recipe, ui_state.temp_user_id); */
                    }
                }
            }
            workers.push(myWorker);
        }
        setWorkerState({ ...worker_state, workers: workers })
    }

    function makeUserID() {
        return <div className="box"> <h2> Please enter your Participant ID: </h2>
            <TextField id="user_id" label="Participant ID" onChange={userIDChange} variant="filled" inputProps={{ inputMode: 'numeric', pattern: '[0-9]*' }} />
            <Button className="button" onClick={submitUserID} variant="contained">Next</Button>
        </div>
    }

    function renderLoading() {

        if (worker_state.working) {
            return <ReactLoading type={"spokes"} color={"1ff"} height={300} width={375} />
        }
    }
    function makeApp() {
        /* if (ui_state.user_id) { */
            return <div className="App">
                <div className="sidebar">
                    <Box className="menu" sx={{ minWidth: 120 }}>
                        <FormControl fullWidth>
                            <InputLabel id="demo-simple-select-label">Mode</InputLabel>
                            <Select
                                labelId="demo-simple-select-label"
                                id="demo-simple-select"
                                value={ui_state.mode}
                                label="Mode"
                                onChange={handleChange}
                            >
                                <MenuItem value={0}>Step by step</MenuItem>
                                <MenuItem value={1}>Write recipe</MenuItem>
                            </Select>
                        </FormControl>
                    </Box>
                    <FormControlLabel
                        label="Continue applying rules"
                        control={
                            <Checkbox defaultChecked checked={ui_state.cont} onChange={() => {
                                const cont = !ui_state.cont;
                                setUIState({ ...ui_state, cont: cont });
                            }} />
                        }
                    />
                    <Box className="menu" sx={{ minWidth: 120 }}>
                        <FormControl fullWidth>
                            <InputLabel id="demo-simple-select-label">X-Axis</InputLabel>
                            <Select
                                labelId="demo-simple-select-label"
                                id="demo-simple-select"
                                value={ui_state.x}
                                label="X-Axis"
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
                                label="Y-Axis"
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
                    {numSamples()}
                    {applyRecipeButton()}
                </div>
                {stepByStep()}
                <div className="vertical">
                    {writeRecipe()}
                    <div className="flex-container">
                        {renderLoading()}
                        {returnSpec(worker_state.graphs)}
                        {renderGraph(worker_state.original_graph, 500, 600)}
                    </div>
                </div>
                {writeButton()}
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
