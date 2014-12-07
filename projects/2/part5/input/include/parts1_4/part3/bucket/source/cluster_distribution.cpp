#include "unit_tests.h"
#include "catch/catch.hpp"
#include "../../../common/priority_queue.h"


SCENARIO( "Verifying the output of cluster_distribution, linear probring" ) {
    GIVEN( "A HashMapBucketsGeneric map with integer keys" ) {
        cop3530::HashMapBucketsGeneric<int, int> map(1000);
        WHEN("The map is filled halfway, cleared, then filled halfway again") {
            for (size_t i = 0; i != 500; ++i)
                map.insert(i, i + 1);
            map.clear();
            for (size_t i = 500; i != 1000; ++i)
                map.insert(i, i + 1);
            THEN("The cluster_distribution reports a list of clusters which reflects the actual number of items in the map") {
                cop3530::priority_queue<cop3530::hash_utils::ClusterInventory> cluster_pq = map.cluster_distribution();
                size_t cluster_item_count = 0;
                while (cluster_pq.size()) {
                    cop3530::hash_utils::ClusterInventory cluster = cluster_pq.get_next_item();
                    cluster_item_count += cluster.cluster_size * cluster.num_instances;
                }
                REQUIRE(cluster_item_count == map.size());
            }
        }
    }

    GIVEN( "A HashMapBucketsGeneric map with double keys" ) {
        cop3530::HashMapBucketsGeneric<double, int> map(1000);
        WHEN("The map is filled halfway, cleared, then filled halfway again") {
            for (size_t i = 0; i != 500; ++i)
                map.insert(static_cast<double>(i) / 2.0, i + 1);
            map.clear();
            for (size_t i = 500; i != 1000; ++i)
                map.insert(static_cast<double>(i) / 2.0, i + 1);
            THEN("The cluster_distribution reports a list of clusters which reflects the actual number of items in the map") {
                cop3530::priority_queue<cop3530::hash_utils::ClusterInventory> cluster_pq = map.cluster_distribution();
                size_t cluster_item_count = 0;
                while (cluster_pq.size()) {
                    cop3530::hash_utils::ClusterInventory cluster = cluster_pq.get_next_item();
                    cluster_item_count += cluster.cluster_size * cluster.num_instances;
                }
                REQUIRE(cluster_item_count == map.size());
            }
        }
    }

    GIVEN( "A HashMapBucketsGeneric map with string keys" ) {
        cop3530::HashMapBucketsGeneric<std::string, int> map(1000);
        WHEN("The map is filled halfway, cleared, then filled halfway again") {
            for (size_t i = 0; i != 500; ++i)
                map.insert(std::to_string(i), i + 1);
            map.clear();
            for (size_t i = 500; i != 1000; ++i)
                map.insert(std::to_string(i), i + 1);
            THEN("The cluster_distribution reports a list of clusters which reflects the actual number of items in the map") {
                cop3530::priority_queue<cop3530::hash_utils::ClusterInventory> cluster_pq = map.cluster_distribution();
                size_t cluster_item_count = 0;
                while (cluster_pq.size()) {
                    cop3530::hash_utils::ClusterInventory cluster = cluster_pq.get_next_item();
                    cluster_item_count += cluster.cluster_size * cluster.num_instances;
                }
                REQUIRE(cluster_item_count == map.size());
            }
        }
    }

    GIVEN( "A HashMapBucketsGeneric map with const char* keys" ) {
        cop3530::HashMapBucketsGeneric<const char*, int> map(1000);
        WHEN("The map is filled halfway, cleared, then filled halfway again") {
            for (size_t i = 0; i != 500; ++i)
                map.insert(std::to_string(i).c_str(), i + 1);
            map.clear();
            for (size_t i = 500; i != 1000; ++i)
                map.insert(std::to_string(i).c_str(), i + 1);
            THEN("The cluster_distribution reports a list of clusters which reflects the actual number of items in the map") {
                cop3530::priority_queue<cop3530::hash_utils::ClusterInventory> cluster_pq = map.cluster_distribution();
                size_t cluster_item_count = 0;
                while (cluster_pq.size()) {
                    cop3530::hash_utils::ClusterInventory cluster = cluster_pq.get_next_item();
                    cluster_item_count += cluster.cluster_size * cluster.num_instances;
                }
                REQUIRE(cluster_item_count == map.size());
            }
        }
    }
}
