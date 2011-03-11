#include <sstream>

#include "../backend/block_backend.h"
#include "../backend/random_file.h"
#include "../core/settings.h"
#include "id_query.h"

using namespace std;

void Id_Query_Statement::set_attributes(const char **attr)
{
  map< string, string > attributes;
  
  attributes["into"] = "_";
  attributes["type"] = "";
  attributes["ref"] = "";
  
  Statement::eval_cstr_array(get_name(), attributes, attr);
  
  output = attributes["into"];
  
  if (attributes["type"] == "node")
    type = Statement::NODE;
  else if (attributes["type"] == "way")
    type = Statement::WAY;
  else if (attributes["type"] == "relation")
    type = Statement::RELATION;
  else
  {
    type = 0;
    ostringstream temp;
    temp<<"For the attribute \"type\" of the element \"id-query\""
	<<" the only allowed values are \"node\", \"way\" or \"relation\".";
    add_static_error(temp.str());
  }
  
  ref = (unsigned int)atol(attributes["ref"].c_str());
  if (ref == 0)
  {
    ostringstream temp;
    temp<<"For the attribute \"ref\" of the element \"id-query\""
	<<" the only allowed values are positive integers.";
    add_static_error(temp.str());
  }
}

void Id_Query_Statement::forecast()
{
/*  Set_Forecast& sf_out(declare_write_set(output));
  
  if (type == NODE)
  {
    sf_out.node_count = 1;
    declare_used_time(400);
  }
  else if (type == WAY)
  {
    sf_out.way_count = 1;
    declare_used_time(300);
  }
  if (type == RELATION)
  {
    sf_out.relation_count = 1;
    declare_used_time(200);
  }
    
  finish_statement_forecast();
    
  display_full();
  display_state();*/
}

void Id_Query_Statement::execute(Resource_Manager& rman)
{
  stopwatch.start();
  
  if (ref == 0)
    return;
  
  map< Uint32_Index, vector< Node_Skeleton > >& nodes
      (rman.sets()[output].nodes);
  map< Uint31_Index, vector< Way_Skeleton > >& ways
      (rman.sets()[output].ways);
  map< Uint31_Index, vector< Relation_Skeleton > >& relations
      (rman.sets()[output].relations);
  map< Uint31_Index, vector< Area_Skeleton > >& areas
      (rman.sets()[output].areas);
  
  nodes.clear();
  ways.clear();
  relations.clear();
  areas.clear();
  
  if (type == NODE)
  {
    set< Uint32_Index > req;
    Uint32_Index idx((uint32)0);
    {
      stopwatch.stop(Stopwatch::NO_DISK);
      Random_File< Uint32_Index > random(*de_osm3s_file_ids::NODES, false);
      idx = random.get(ref);
      stopwatch.stop(Stopwatch::NODES_MAP);
    }
    req.insert(idx);
    
    stopwatch.stop(Stopwatch::NO_DISK);
    Block_Backend< Uint32_Index, Node_Skeleton > nodes_db
	(*de_osm3s_file_ids::NODES, false);
    for (Block_Backend< Uint32_Index, Node_Skeleton >::Discrete_Iterator
	 it(nodes_db.discrete_begin(req.begin(), req.end()));
	 !(it == nodes_db.discrete_end()); ++it)
    {
      if (it.object().id == ref)
	nodes[it.index()].push_back(it.object());
    }
    stopwatch.add(Stopwatch::NODES, nodes_db.read_count());
    stopwatch.stop(Stopwatch::NODES);
  }
  else if (type == WAY)
  {
    set< Uint31_Index > req;
    Uint31_Index idx((uint32)0);
    {
      stopwatch.stop(Stopwatch::NO_DISK);
      Random_File< Uint31_Index > random(*de_osm3s_file_ids::WAYS, false);
      idx = random.get(ref);
      stopwatch.stop(Stopwatch::WAYS_MAP);
    }
    req.insert(idx);

    stopwatch.stop(Stopwatch::NO_DISK);
    Block_Backend< Uint31_Index, Way_Skeleton > ways_db
	(*de_osm3s_file_ids::WAYS, false);
    for (Block_Backend< Uint31_Index, Way_Skeleton >::Discrete_Iterator
	 it(ways_db.discrete_begin(req.begin(), req.end()));
	 !(it == ways_db.discrete_end()); ++it)
    {
      if (it.object().id == ref)
	ways[it.index()].push_back(it.object());
    }
    stopwatch.add(Stopwatch::WAYS, ways_db.read_count());
    stopwatch.stop(Stopwatch::WAYS);
  }
  else if (type == RELATION)
  {
    set< Uint31_Index > req;
    Uint31_Index idx((uint32)0);
    {
      stopwatch.stop(Stopwatch::NO_DISK);
      Random_File< Uint31_Index > random(*de_osm3s_file_ids::RELATIONS, false);
      idx = random.get(ref);
      stopwatch.stop(Stopwatch::RELATIONS_MAP);
    }
    req.insert(idx);

    stopwatch.stop(Stopwatch::NO_DISK);
    Block_Backend< Uint31_Index, Relation_Skeleton > relations_db
	(*de_osm3s_file_ids::RELATIONS, false);
    for (Block_Backend< Uint31_Index, Relation_Skeleton >::Discrete_Iterator
	 it(relations_db.discrete_begin(req.begin(), req.end()));
	 !(it == relations_db.discrete_end()); ++it)
    {
      if (it.object().id == ref)
	relations[it.index()].push_back(it.object());
    }
    stopwatch.add(Stopwatch::RELATIONS, relations_db.read_count());
    stopwatch.stop(Stopwatch::RELATIONS);
  }
  
  stopwatch.report(get_name());
  rman.health_check(*this);
}