#include "trianglemesh.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <fstream>


// Constructor of a triangle mesh.
TriangleMesh::TriangleMesh()
{
	numVertices = 0;
	numTriangles = 0;
	objCenter = glm::vec3(0.0f, 0.0f, 0.0f);
	objExtent = glm::vec3(0.0f, 0.0f, 0.0f);	
}

// Destructor of a triangle mesh.
TriangleMesh::~TriangleMesh()
{
	vertices.clear();
	vertexIndices.clear();	
}

unsigned int TriangleMesh::getIndex(int n) {
	if (n <= 0) {
		return numVertices + n;
	}
	return n - 1;
}
// Load the geometry and material data from an OBJ file.
bool TriangleMesh::LoadFromFile(const std::string& filePath, const bool normalized)
{	
	std::ifstream OBJfile(filePath);
	std::string s;
	if (!OBJfile.is_open()) {		
		return false;
	}

	while (std::getline(OBJfile, s)) {
		std::string vertex_info;
		std::stringstream data;
		data << s;
		data >> vertex_info;
		glm::vec3 vtx = { 0,0,0 };
		glm::vec3 n = { 0,0,0 };
		glm::vec2 t = { 0,0 };
		int index = 0;
		if (vertex_info == "v") {
			float pos;
			while (data >> pos) {
				vtx[index] = pos;
				index++;
			}
			numVertices++;
			vertices.push_back(VertexPTN(vtx, n, t));
		}		

		if (vertex_info == "f") {
			std::vector<std::string> str;
			std::string str2;
			int num = 0, vertex = 0;
			while (data >> str2) {
				str.push_back(str2);
				str2.clear();
			}
			for (int i = 0;i < str.size();i++) {
				for (int j = 0;j < str[i].length();j++) {
					if (str[i][j] == '/') {
						str[i][j] = ' ';
					}
				}
			}
			std::stringstream str_to_int;
			int a;
			str_to_int << str[0];
			str_to_int >> a;

			for (int i = 1;i < str.size() - 1;i++) {
				int num1 = 0, num2 = 0;
				str_to_int.str("");
				str_to_int.clear();
				str_to_int << str[i];
				str_to_int >> num1;
				str_to_int.str("");
				str_to_int.clear();
				str_to_int << str[i + 1];
				str_to_int >> num2;
				vertexIndices.push_back(getIndex(a));
				vertexIndices.push_back(getIndex(num1));
				vertexIndices.push_back(getIndex(num2));
				numTriangles++;
			}			
		}		
	}
	
	if (normalized) {		
		glm::vec3 max_pos = vertices[0].position, min_pos = vertices[0].position, length = {0,0,0};
		for (int i = 0;i < numVertices;i++) {
			max_pos = glm::max(max_pos, vertices[i].position);
			min_pos = glm::min(min_pos, vertices[i].position);
		}

		objCenter = (max_pos + min_pos) / 2.0f;
		for (int i = 0;i < numVertices;i++) {
			vertices[i].position -= objCenter;
		}
		objCenter = { 0,0,0 };

		objExtent = max_pos - min_pos;		
		for (int i = 0;i < numVertices;i++) {
			vertices[i].position /= objExtent;
		}
		objExtent = { 1,1,1 };
	}	
	return true;
}

// Apply transform to vertices using GPU.
void TriangleMesh::ApplyTransformCPU(const glm::mat4x4& mvpMatrix)
{
	for (int i = 0; i < numVertices; ++i) {
		glm::vec4 p = mvpMatrix * glm::vec4(vertices[i].position, 1.0f);
		if (p.w != 0.0f) {
			float inv = 1.0f / p.w;
			vertices[i].position.x = p.x * inv;
			vertices[i].position.y = p.y * inv;
			vertices[i].position.z = p.z * inv;
		}
	}
}

// Show model information.
void TriangleMesh::ShowInfo()
{
	std::cout << "# Vertices: " << numVertices << std::endl;
	std::cout << "# Triangles: " << numTriangles << std::endl;
	std::cout << "Model Center: " << objCenter.x << ", " << objCenter.y << ", " << objCenter.z << std::endl;
	std::cout << "Model Extent: " << objExtent.x << " x " << objExtent.y << " x " << objExtent.z << std::endl;
}
