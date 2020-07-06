#include "stdafx.h"
#include "SphereTreeBuilder.h"
#include <EngineCore\Logger.h>
#include <miniball\Seb.h>

namespace physics
{
	SphereTreeCollisionHull* SphereTreeBuilder::generateCollisionHull(const std::vector<unsigned int> &indexed_mesh, const std::vector<glm::vec3> &vertices)
	{
		std::vector<SphereTreeNode*> leaf_nodes;

		coverSurface(indexed_mesh, vertices, leaf_nodes);
		
		SphereTreeNode* root = constructTree(leaf_nodes);

		SphereTreeCollisionHull* collision_hull = new SphereTreeCollisionHull(root);

		refitSpheres(root, vertices);

		/*SphereTreeNode* root = constructLayers(leaf_nodes, left_subset_used_last);
		root->flag = SphereTreeNode::ROOT;*/
				
		

		//refitLayers(vertices, root);

		return collision_hull;
	} 

	void SphereTreeBuilder::coverSurface(const std::vector<unsigned int> &indexed_mesh, const std::vector<glm::vec3> &vertices, std::vector<SphereTreeNode*> &leaf_nodes)
	{
		for (int i = 0; i < indexed_mesh.size(); i += 3)
		{
			glm::vec3 p1 = vertices[indexed_mesh[i]];
			glm::vec3 p2 = vertices[indexed_mesh[i + 1]];
			glm::vec3 p3 = vertices[indexed_mesh[i + 2]];

			glm::vec3 edge_a = p2 - p1;
			float length_a = glm::length(edge_a);
			glm::vec3 edge_b = p3 - p1;
			float length_b = glm::length(edge_b);
			glm::vec3 edge_c = p3 - p2;
			float length_c = glm::length(edge_c);

			glm::vec3 edge_a_origin = p1;
			glm::vec3 edge_b_origin = p1;
			glm::vec3 edge_c_origin = p2;

			// make edge_c the longest edge of the triangle
			if (length_c < length_a)
			{
				glm::vec3 temp_edge = edge_c;
				float temp_length = length_c;
				glm::vec3 temp_edge_origin = edge_c_origin;

				edge_c = edge_a;
				length_c = length_a;
				edge_c_origin = edge_a_origin;

				edge_a = temp_edge;
				length_a = temp_length;
				edge_a_origin = temp_edge_origin;
			}

			if (length_c < length_b)
			{
				glm::vec3 temp_edge = edge_c;
				float temp_length = length_c;
				glm::vec3 temp_edge_origin = edge_c_origin;

				edge_c = edge_b;
				length_c = length_b;
				edge_c_origin = edge_b_origin;

				edge_b = temp_edge;
				length_b = temp_length;
				edge_b_origin = temp_edge_origin;
			}

			glm::vec3 center;
			float radius;

			// triangle is obtuse or right
			if ((length_a * length_a) + (length_b * length_b) <= (length_c * length_c))
			{
				center = edge_c_origin + (edge_c * 0.5f);
				radius = length_c / 2.f;
			}
			else // triangle is acute
			{
				// circumscribed circle (sphere)

				glm::vec3 p1p2 = glm::vec3(p1 - p2), p2p3 = glm::vec3(p2 - p3), p3p1 = glm::vec3(p3 - p1);
				glm::vec3 p1p3 = glm::vec3(p1 - p3), p2p1 = glm::vec3(p2 - p1), p3p2 = glm::vec3(p3 - p2);

				// circumscribed circle (sphere) - radius
				// see https://en.wikipedia.org/wiki/Circumscribed_circle 
				// -> Cartesian coordinates from cross- and dot-products
				glm::vec3 cross = glm::cross(p1p2, p2p3);
				float length_cross = glm::length(cross);

				float numerator = glm::length(glm::vec3(p1p2)) * glm::length(glm::vec3(p2p3)) * glm::length(glm::vec3(p3p1));
				float denominator = 2.f * length_cross;

				radius = numerator / denominator;

				// circumscribed circle (sphere) - center
				float length_p2p3_square = glm::length(p2p3) * glm::length(p2p3);
				float length_p1p3_square = glm::length(p1p3) * glm::length(p1p3);
				float length_p1p2_square = glm::length(p1p2) * glm::length(p1p2);
				float double_length_cross_square = 2.f * (length_cross * length_cross);

				float alpha = (length_p2p3_square * glm::dot(p1p2, p1p3)) / double_length_cross_square;
				float beta = (length_p1p3_square * glm::dot(p2p1, p2p3)) / double_length_cross_square;
				float gamma = (length_p1p2_square * glm::dot(p3p1, p3p2)) / double_length_cross_square;

				center = (alpha * p1) + (beta * p2) + (gamma * p3);
			}

			SphereTreeNode* node = new SphereTreeNode(NULL, center, radius, SphereTreeNode::LEAF);
			node->original_center = center;
			node->original_radius = radius;

			// Partikel hier speichern
			node->particle_ids.push_back(indexed_mesh[i]);
			node->particle_ids.push_back(indexed_mesh[i + 1]);
			node->particle_ids.push_back(indexed_mesh[i + 2]);

			leaf_nodes.push_back(node);		
		}

		
	}

	SphereTreeNode * SphereTreeBuilder::constructTree(std::vector<SphereTreeNode*>& leafs)
	{
		SphereTreeNode *root = new SphereTreeNode(nullptr, glm::vec3(0), 0, SphereTreeNode::INNER_NODE);
		
		insertNodes(root, root, leafs);

		root->flag = SphereTreeNode::ROOT;

		return root;
	}

	void SphereTreeBuilder::insertNodes(SphereTreeNode* &root, SphereTreeNode* node, std::vector<SphereTreeNode*> &leaves)
	{
		if (leaves.size() <= 8)
		{
			for (int i = 0; i < leaves.size(); i++)
			{
				node->children[i] = leaves[i];
				node->children[i]->parent = node;
			}
		}
		else
		{
			std::vector<SphereTreeNode*> groups[8];

			glm::vec3 max = leaves[0]->center;
			glm::vec3 min = leaves[1]->center;

			for (int i = 1; i < leaves.size(); i++)
			{
				max.x = glm::max(max.x, leaves[i]->center.x);
				max.y = glm::max(max.y, leaves[i]->center.y);
				max.z = glm::max(max.z, leaves[i]->center.z);
				min.x = glm::min(min.x, leaves[i]->center.x);
				min.y = glm::min(min.y, leaves[i]->center.y);
				min.z = glm::min(min.z, leaves[i]->center.z);
			}

			glm::vec3 center = min + (max - min) / 2.0f;

			for (int i = 0; i < leaves.size(); i++)
			{
				int target = 0;
				target |= leaves[i]->center.x < center.x ? 0 : 1;
				target |= leaves[i]->center.y < center.y ? 0 : 2;
				target |= leaves[i]->center.z < center.z ? 0 : 4;

				groups[target].push_back(leaves[i]);
			}

			int i_insert = 0;

			for (int i = 0; i < 8; i++)
			{
				if (groups[i].size() > 0)
				{
					node->children[i_insert] = new SphereTreeNode(node, glm::vec3(0), 0, SphereTreeNode::INNER_NODE);
					node->children[i_insert]->flag = SphereTreeNode::INNER_NODE;
					insertNodes(root, node->children[i_insert], groups[i]);
					i_insert++;
				}
			}
		}
	}

	void SphereTreeBuilder::refitSpheres(SphereTreeNode * node, const std::vector<glm::vec3> &vertices)
	{
		if (node->flag == SphereTreeNode::LEAF)
		{
			node->original_radius = node->radius;
			node->original_center = node->center;
			return;
		}

		for (int i = 0; i < SphereTreeNode::num_children; i++)
		{
			if (node->children[i] != NULL)
			{
				refitSpheres(node->children[i], vertices);
				node->particle_ids.insert(node->particle_ids.end(), node->children[i]->particle_ids.begin(), node->children[i]->particle_ids.end());
			}
		}

		std::sort(node->particle_ids.begin(), node->particle_ids.end());
		node->particle_ids.erase(std::unique(node->particle_ids.begin(), node->particle_ids.end()), node->particle_ids.end());

		//miniball
		{
			typedef Seb::Point<float> Point;
			typedef std::vector<Point> PointVector;
			typedef Seb::Smallest_enclosing_ball<float> Miniball;
			
			PointVector S;
			std::vector<float> coords(3);

			for (int i = 0; i < node->particle_ids.size(); ++i)
			{
				coords[0] = vertices[node->particle_ids[i]].x;
				coords[1] = vertices[node->particle_ids[i]].y;
				coords[2] = vertices[node->particle_ids[i]].z;
			
				S.push_back(Point(3,coords.begin()));
			}

			Miniball mb(3, S);
		
			node->radius = mb.radius();
			float *f = mb.center_begin();
			node->center.x = f[0];
			node->center.y = f[1];
			node->center.z = f[2];
		
		}

		node->original_radius = node->radius;
		node->original_center = node->center;

	}

	SphereTreeNode* SphereTreeBuilder::constructLayers(const std::vector<SphereTreeNode*> &origin_set, bool &left_subset_used_last)
	{
		std::vector<SphereTreeNode*> left_subset;
		std::vector<SphereTreeNode*> right_subset;

		if (origin_set.size() == 1)
			return origin_set[0];
		
		SphereTreeNode* left_child = NULL;
		SphereTreeNode* right_child = NULL;

		glm::vec3 min(0.f), max(0.f);

		generateBoundingBox(origin_set, min, max);

		int longest_dim = 0;

		findLongestAxis(min, max, longest_dim);

		float avrg = 0.f;

		for (int i = 0; i < origin_set.size(); i++)
			avrg += origin_set[i]->center[longest_dim];

		avrg /= origin_set.size();

		for (int i = 0; i < origin_set.size(); i++)
		{
			if (origin_set[i]->center[longest_dim] > avrg)
			{
				//if (origin_set.size() == 2 /*&& origin_set[0]->center[longest_dim] == origin_set[1]->center[longest_dim]*/)
				//{
				//	left_subset.push_back(origin_set[0]);
				//	right_subset.push_back(origin_set[1]);
				//	break;
				//}
				//else
				left_subset.push_back(origin_set[i]);
			}
			else if (origin_set[i]->center[longest_dim] < avrg)
			{
				//if (origin_set.size() == 2 /*&& origin_set[0]->center[longest_dim] == origin_set[1]->center[longest_dim]*/)
				//{
				//	left_subset.push_back(origin_set[0]);
				//	right_subset.push_back(origin_set[1]);
				//	break;
				//}
				//else
				right_subset.push_back(origin_set[i]);
			}
			else
			{
				if (!left_subset_used_last)
				{
					left_subset.push_back(origin_set[i]);
					left_subset_used_last = true;
				}
				else
				{
					right_subset.push_back(origin_set[i]);
					left_subset_used_last = false;
				}
			}
		}

		//FIXES
		if (right_subset.size() == 0)
		{
			core::Logger::warning("Sphere Tree Builder failure! Unbalanced: All left");

			SphereTreeNode* l = left_subset[left_subset.size() - 1];
			right_subset.push_back(l);
			left_subset.pop_back();
		}
		else if (left_subset.size() == 0)
		{
			core::Logger::warning("Sphere Tree Builder failure! Unbalanced: All right");

			SphereTreeNode* l = right_subset[right_subset.size() - 1];
			left_subset.push_back(l);
			right_subset.pop_back();
		}
		//END: FIXES

		left_child = constructLayers(left_subset, left_subset_used_last);
		right_child = constructLayers(right_subset, left_subset_used_last);
		
		glm::vec3 center_children(0.f);
		glm::vec3 center_leaves(0.f);
		float radius_children = 0.f;
		float radius_leaves = 0.f;

		generateSphereFromChildren(left_child, right_child, center_children, radius_children);
		generateSphereFromLeaves(left_child, right_child, center_leaves, radius_leaves);

		glm::vec3 center(0.f);
		float radius = 0.f;

		if (radius_children < radius_leaves)
		{
			center = center_children;
			radius = radius_children;
		}
		else
		{
			center = center_leaves;
			radius = radius_leaves;
		}

		SphereTreeNode* layer_node = new SphereTreeNode(NULL, center, radius, SphereTreeNode::INNER_NODE);
		layer_node->original_center = center;
		layer_node->original_radius = radius;

		left_child->parent = layer_node;
		right_child->parent = layer_node;
		
		layer_node->children[0] = left_child;
		layer_node->children[1] = right_child;

		// save particles of descendent nodes for refitting
		for (int i = 0; i < layer_node->children[0]->particle_ids.size(); i++)
		{
			layer_node->particle_ids.push_back(layer_node->children[0]->particle_ids[i]);
		}

		for (int i = 0; i < layer_node->children[1]->particle_ids.size(); i++)
		{
			layer_node->particle_ids.push_back(layer_node->children[1]->particle_ids[i]);
		}

		return layer_node;
	}

	void SphereTreeBuilder::generateSphereFromChildren(SphereTreeNode* left_child, SphereTreeNode* right_child, glm::vec3 &center, float &radius)
	{
		glm::vec3 AB_half = (right_child->center - left_child->center) / 2.f;

		center = AB_half + left_child->center;

		float max_radius = left_child->radius > right_child->radius ? left_child->radius : right_child->radius;

		radius = glm::length(AB_half) + max_radius;
	}

	void SphereTreeBuilder::generateSphereFromLeaves(SphereTreeNode* left_child, SphereTreeNode* right_child, glm::vec3 &center, float &radius)
	{
		std::vector<SphereTreeNode*> leaves;

		findLeafSpheres(left_child, right_child, leaves);

		glm::vec3 avrg(0.f);

		for (int i = 0; i < leaves.size(); i++)
			avrg += leaves[i]->center;
			
		avrg /= 2.f;
		center = avrg;

		// Radius nach Methode 2 berechnen
		float max_dist = 0.f;
		int index = 0;

		for (int i = 0; i < leaves.size(); i++)
		{
			glm::vec3 AB = leaves[i]->center - center;

			if (glm::length(AB) > max_dist)
			{
				max_dist = glm::length(AB);
				index = i;
			}
		}

		radius = max_dist + leaves[index]->radius;
	}

	void SphereTreeBuilder::findLeafSpheres(SphereTreeNode* left_child, SphereTreeNode* right_child, std::vector<SphereTreeNode*> &leaves)
	{
		if (left_child->flag == SphereTreeNode::LEAF)
			leaves.push_back(left_child);
		else
			findLeafSpheres(left_child->children[0], left_child->children[1], leaves);

		if (right_child->flag == SphereTreeNode::LEAF)
			leaves.push_back(right_child);
		else
			findLeafSpheres(right_child->children[0], right_child->children[1], leaves);
	}

	void SphereTreeBuilder::refitLayers(const std::vector<glm::vec3> &vertices, SphereTreeNode* node)
	{
		float min_rad = 0.f;

		if (node->flag == SphereTreeNode::ROOT || node->flag == SphereTreeNode::INNER_NODE)
		{
			for (int i = 0; i < node->particle_ids.size(); i++)
			{
				glm::vec3 dist_vec = glm::vec3(vertices[node->particle_ids[i]] - node->center);
				float dist = glm::length(dist_vec);

				if (node->radius > dist)
				{
					min_rad = dist;
				}
				else
				{
					min_rad = node->radius;
				}

				//min_rad = node->radius > dist ? dist : node->radius;
			}
			node->radius = min_rad;
		}

		if (node->children[0] != NULL)
			refitLayers(vertices, node->children[0]);

		if (node->children[1] != NULL)
			refitLayers(vertices, node->children[1]);
	}

	void SphereTreeBuilder::generateBoundingBox(const std::vector<SphereTreeNode*> &nodes, glm::vec3 &min, glm::vec3 &max) {
		float x_min = 0.f, x_max = 0.f, y_min = 0.f, y_max = 0.f, z_min = 0.f, z_max = 0.f;

		for (int i = 0; i < nodes.size(); i++)
		{
			x_min = x_min > nodes[i]->center.x ? nodes[i]->center.x : x_min;
			x_max = x_max < nodes[i]->center.x ? nodes[i]->center.x : x_max;

			y_min = y_min > nodes[i]->center.y ? nodes[i]->center.y : y_min;
			y_max = y_max < nodes[i]->center.y ? nodes[i]->center.y : y_max;

			z_min = z_min > nodes[i]->center.z ? nodes[i]->center.z : z_min;
			z_max = z_max < nodes[i]->center.z ? nodes[i]->center.z : z_max;
		}

		min = glm::vec3(x_min, y_min, z_min);
		max = glm::vec3(x_max, y_max, z_max);
	}

	void SphereTreeBuilder::findLongestAxis(const glm::vec3 &min, const glm::vec3 &max, int &longest_dim)
	{
		float dimensions[3];
		float dim_max = 0.f;
		glm::vec3 axis[3];

		axis[0] = glm::vec3(max.x, min.y, min.z) - min; // x
		axis[1] = glm::vec3(min.x, max.y, min.z) - min; // y
		axis[2] = glm::vec3(min.x, min.y, max.z) - min; // z

		dimensions[0] = glm::length(axis[0]);
		dimensions[1] = glm::length(axis[1]);
		dimensions[2] = glm::length(axis[2]);

		for (int i = 0; i < 3; i++)
		{
			if (dim_max < dimensions[i])
			{
				dim_max = dimensions[i];
				longest_dim = i;
			}
		}
	}
}