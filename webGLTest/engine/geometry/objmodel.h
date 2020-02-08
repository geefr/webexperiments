#ifndef OBJMODEL_H
#define OBJMODEL_H

#include "geometry.h"

#include <string>
#include <vector>

class ObjModel : public Geometry
{
public:
		ObjModel( std::string objFile );
		virtual ~ObjModel();
		
private:
		struct FaceIndex {
			GLuint vert;
			GLuint tex;
			GLuint norm;
		};
		using Face = std::array<FaceIndex,3>;

		bool load();
		void objTexCoord(std::string line);
		void objVertNormal(std::string line);
		void objFace(std::string line);
		void objVert(std::string line);
		std::string mObjFile;

		std::vector<glm::vec3> mVertCoords;
		std::vector<glm::vec2> mVertTexCoords;
		std::vector<glm::vec3> mVertNorms;
		std::vector<glm::vec<3,GLuint>> mObjIndices;
};

#endif
