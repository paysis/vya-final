#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <yaml-cpp/yaml.h>

using namespace std;

class Activity {
public:
    string id;
    string name;
    int duration;
    vector<Activity*> predecessors;
    unordered_set<Activity*> checked_predecessors;
    unordered_set<Activity*> checked_successors;
    // unordered_map<Activity*, int> checked_predecessors;
    vector<Activity*> successors;
    int earliestStartTime;
    int earliestFinishTime;
    int latestStartTime;
    int latestFinishTime;
    int slack;

    int maxOfPredecessors;
    int minOfSuccessors;

    Activity(string id){
        this->id = id;
        this->name = id;
        this->duration = -1;
        earliestStartTime = earliestFinishTime = latestStartTime = latestFinishTime = slack = maxOfPredecessors = 0;
        minOfSuccessors = -1;
    }

    Activity(string id, string name, int duration) {
        this->id = id;
        this->name = name;
        this->duration = duration;
        earliestStartTime = earliestFinishTime = latestStartTime = latestFinishTime = slack = maxOfPredecessors = 0;
        minOfSuccessors = -1;
    }
};

class Graph {
public:
    vector<Activity*> vertices;
    int earliestTotalFinishTime;
    // unordered_set<Activity*> checked_vertices;

    Graph() {
        earliestTotalFinishTime = 0;
    }

    void addActivity(Activity* activity) {
        vertices.push_back(activity);
    }

    void calculateET(){
        queue<Activity*> q;
        for(auto& v : vertices){
            if (v->predecessors.empty()){
                v->earliestStartTime = 0;
                v->earliestFinishTime = v->duration;
                q.push(v);
            }
        }

        while(!q.empty()){
            Activity* v = q.front();
            q.pop();
            /*
                Iterating through children of v, we check if they have any predecessors left.
                If they do, we set the maxOfPredecessors to be maximum of itself or
                current v's earliest finish time.

                Once there are no predecessors left, we set the earliest start time to be maxOfPredecessors
                and the earliest finish time to be maxOfPredecessors + duration of that child.

                This should work as at the end of the day, we should have the maximum of all predecessors
                which would give us the earliest finish time of a group of activities.
            */
            for(auto& u : v->successors){
                // u->checked_predecessors[v] = v->earliestFinishTime;
                if(u->duration == -1){
                    cout << "Error: Activity " << u->id << " has no duration." << endl;
                    cout << "Please check the YAML file." << endl;
                    exit(1);
                }
                u->checked_predecessors.insert(v);
                u->maxOfPredecessors = max(
                    u->maxOfPredecessors,
                    v->earliestFinishTime
                );
                if(u->checked_predecessors.size() == u->predecessors.size()){
                    u->earliestStartTime = u->maxOfPredecessors;
                    u->earliestFinishTime = u->maxOfPredecessors + u->duration;
                    this->earliestTotalFinishTime = max(
                        u->earliestFinishTime,
                        this->earliestTotalFinishTime
                    );
                    q.push(u);
                }
            }
        }
    }

    void calculateLT(){
        queue<Activity*> q;
        for(auto& v : vertices){
            if(v->successors.empty()){
                v->latestFinishTime = this->earliestTotalFinishTime;
                v->latestStartTime = v->latestFinishTime - v->duration;
                q.push(v);
            }
        }

        while(!q.empty()){
            Activity* v = q.front();
            q.pop();

            for(auto& u : v->predecessors){
                u->checked_successors.insert(v);

                // Eğer minOfSuccessors -1 değilse, yani bir değer almışsa, o değer ile
                // şu anki v'nin latestStartTime'ı arasından en küçüğünü alıyoruz.
                // Eğer -1 ise, yani bir değer almamışsa, şu anki v'nin latestStartTime'ını alıyoruz.
                u->minOfSuccessors = u->minOfSuccessors != -1 ? min(
                    u->minOfSuccessors,
                    v->latestStartTime
                ) : v->latestStartTime;

                if(u->checked_successors.size() == u->successors.size()){
                    u->latestFinishTime = u->minOfSuccessors;
                    u->latestStartTime = u->latestFinishTime - u->duration;
                    q.push(u);
                }
            }

            v->slack = v->latestStartTime - v->earliestStartTime;

        }
    }

    void printCriticalPath() {
        cout << "Critical Path: ";
        bool firstOne = true;
        for (auto& v : vertices) {
            if (v->slack == 0) {
                if(!firstOne){
                    cout << " -> ";
                } // to make output look a little bit more pretty
                
                cout << "(" << v->id << ") " << v->name;
                firstOne = false;
            }
            
        }
        cout << "." << endl;
    }
};

class YAMLParser{
    public:
    YAMLParser(string filename){
        this->filename = filename;
        try{
            this->data = YAML::LoadFile(filename);
        }catch(YAML::Exception& e){
            cout << "File loading error: " << e.what() << endl;
            exit(1);
        }
    }
    
    Graph* parse(){
        Graph* graph = new Graph();

        try{
            auto activities_raw = this->data["activities"];
            for(size_t i = 0; i < activities_raw.size(); ++i){
                const YAML::Node& activity_raw = activities_raw[i];

                string id = activity_raw["id"].as<string>();
                string name = activity_raw["name"].as<string>();
                int duration = activity_raw["duration"].as<int>();

                if(duration < 0){
                    cout << "Error: Activity " << id << " has a negative duration." << endl;
                    cout << "Please check the YAML file." << endl;
                    exit(1);
                }

                Activity* activity;
                if(activities_table.find(id) == activities_table.end()){
                    activity = new Activity(id, name, duration);
                }else{
                    activity = activities_table[id];
                    activity->name = name;
                    activity->duration = duration;
                }

                graph->addActivity(activity);

                const YAML::Node& dependencies_raw = activity_raw["dependencies"];

                for(size_t j = 0; j < dependencies_raw.size(); ++j){
                    const YAML::Node& dependency_raw = dependencies_raw[j];
                    string dependency_id = dependency_raw.as<string>();

                    if(activities_table.find(dependency_id) == activities_table.end()){
                        Activity* dependency = new Activity(dependency_id);
                        activities_table[dependency_id] = dependency;
                    }
                    Activity* dependency = activities_table[dependency_id];

                    activity->predecessors.push_back(dependency);
                    dependency->successors.push_back(activity);
                }

                activities_table[id] = activity;
            }
        }catch(YAML::Exception& e){
            cout << "File parsing error: " << e.what() << endl;
            exit(1);
        }

        return graph;
    }

    private:
    string filename;
    YAML::Node data;
    unordered_map<string, Activity*> activities_table;
    unordered_set<string> vertices_set;
};

int main(int argc, char** argv) {

    string argv1_str(argv[1]);
    Graph* g = YAMLParser("../shared/" + argv1_str).parse();

    // Activity* a = new Activity("A", 7);
    // Activity* b = new Activity("B", 9);
    // Activity* c = new Activity("C", 12);
    // Activity* d = new Activity("D", 8);
    // Activity* e = new Activity("E", 9);
    // Activity* f = new Activity("F", 6);
    // Activity* g1 = new Activity("G", 5);

    // a->successors.push_back(c);
    // c->predecessors.push_back(a);

    // a->successors.push_back(d);
    // d->predecessors.push_back(a);

    // b->successors.push_back(d);
    // d->predecessors.push_back(b);

    // c->successors.push_back(f);
    // f->predecessors.push_back(c);

    // d->successors.push_back(e);
    // e->predecessors.push_back(d);

    // e->successors.push_back(f);
    // f->predecessors.push_back(e);

    // e->successors.push_back(g1);
    // g1->predecessors.push_back(e);

    // g.addActivity(a);
    // g.addActivity(b);
    // g.addActivity(c);
    // g.addActivity(d);
    // g.addActivity(e);
    // g.addActivity(f);
    // g.addActivity(g1);

    g->calculateET();
    g->calculateLT();

    // Display

    // show SPCT (?)

    // print each vertice's name, duration, ES, EF, LS, LF, slack with tabs
    // print critical path
    g->printCriticalPath();

    return 0;
}