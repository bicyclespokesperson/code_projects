// Model class based on: https://learnopengl.com/Model-Loading/Model
#ifndef MODEL_H
#define MODEL_H

#include <mesh.h>
//#include <learnopengl/shader.h>

uint32_t texture_from_file(const char* path, const std::string& directory, bool gamma = false);

class Model {
 public:
  std::vector<Texture> m_textures_loaded;  // stores all the textures loaded so far, optimization to make sure textures
                                           // aren't loaded more than once.
  std::vector<Mesh> m_meshes;
  std::string m_directory;
  bool m_gamma_correction;

  /*  Functions   */
  // constructor, expects a filepath to a 3D model.
  Model(std::string const& path, bool gamma = false) : m_gamma_correction(gamma) { load_model(path); }

  // draws the model, and thus all its meshes
#if 0
    void Draw(Shader shader)
    {
        for(uint32_t i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }
#endif

 private:
  // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
  void load_model(std::string const& path) {
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene =
        importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)  // if is Not Zero
    {
      std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
      return;
    }
    // retrieve the directory path of the filepath
    m_directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    process_node(scene->mRootNode, scene);
  }

  // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this
  // process on its children nodes (if any).
  void process_node(aiNode* node, const aiScene* scene) {
    // process each mesh located at the current node
    for (uint32_t i = 0; i < node->mNumMeshes; i++) {
      // the node object only contains indices to index the actual objects in the scene.
      // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
      aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
      m_meshes.push_back(process_mesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (uint32_t i = 0; i < node->mNumChildren; i++) {
      process_node(node->mChildren[i], scene);
    }
  }

  Mesh process_mesh(aiMesh* mesh, const aiScene* scene) {
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Texture> textures;

    // Walk through each of the mesh's vertices
    for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
      Vertex vertex;
      glm::vec3
          placeholder;  // we declare a placeholder vector since assimp uses its own vector class that doesn't directly
                        // convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
      // positions
      placeholder.x = mesh->mVertices[i].x;
      placeholder.y = mesh->mVertices[i].y;
      placeholder.z = mesh->mVertices[i].z;
      vertex.Position = placeholder;
      // normals
      placeholder.x = mesh->mNormals[i].x;
      placeholder.y = mesh->mNormals[i].y;
      placeholder.z = mesh->mNormals[i].z;
      vertex.Normal = placeholder;
      // texture coordinates
      if (mesh->mTextureCoords[0])  // does the mesh contain texture coordinates?
      {
        glm::vec2 vec;
        // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
        // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
        vec.x = mesh->mTextureCoords[0][i].x;
        vec.y = mesh->mTextureCoords[0][i].y;
        vertex.TexCoords = vec;
      } else
        vertex.TexCoords = glm::vec2(0.0f, 0.0f);
      // tangent
      placeholder.x = mesh->mTangents[i].x;
      placeholder.y = mesh->mTangents[i].y;
      placeholder.z = mesh->mTangents[i].z;
      vertex.Tangent = placeholder;
      // bitangent
      placeholder.x = mesh->mBitangents[i].x;
      placeholder.y = mesh->mBitangents[i].y;
      placeholder.z = mesh->mBitangents[i].z;
      vertex.Bitangent = placeholder;
      vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex
    // indices.
    for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
      aiFace face = mesh->mFaces[i];
      // retrieve all indices of the face and store them in the indices vector
      for (uint32_t j = 0; j < face.mNumIndices; j++) {
        indices.push_back(face.mIndices[j]);
      }
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    std::vector<Texture> diffuseMaps = load_material_textures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    std::vector<Texture> specularMaps = load_material_textures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<Texture> normalMaps = load_material_textures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<Texture> heightMaps = load_material_textures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    // return a mesh object created from the extracted mesh data
    return Mesh(vertices, indices, textures);
  }

  // checks all material textures of a given type and loads the textures if they're not loaded yet.
  // the required info is returned as a Texture struct.
  std::vector<Texture> load_material_textures(aiMaterial* mat, aiTextureType type, std::string typeName) {
    std::vector<Texture> textures;
    for (uint32_t i = 0; i < mat->GetTextureCount(type); i++) {
      aiString str;
      mat->GetTexture(type, i, &str);
      // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
      bool skip = false;
      for (uint32_t j = 0; j < m_textures_loaded.size(); j++) {
        if (std::strcmp(m_textures_loaded[j].path.data(), str.C_Str()) == 0) {
          textures.push_back(m_textures_loaded[j]);
          skip =
              true;  // a texture with the same filepath has already been loaded, continue to next one. (optimization)
          break;
        }
      }
      if (!skip) {  // if texture hasn't been loaded already, load it
        Texture texture;
        texture.id = texture_from_file(str.C_Str(), this->m_directory);
        texture.type = typeName;
        texture.path = str.C_Str();
        textures.push_back(texture);
        m_textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't
                                               // unnecesery load duplicate textures.
      }
    }
    return textures;
  }
};

uint32_t texture_from_file(const char* path, const std::string& directory, bool gamma) {
  std::string filename = std::string(path);
  filename = directory + '/' + filename;

  uint32_t texture_id;
#if 0
    glGenTextures(1, &texture_id);
#endif

  int width, height, nr_components;
  uint8_t* data = stbi_load(filename.c_str(), &width, &height, &nr_components, 0);
  if (data) {
#if 0
    GLenum format;
    if (nr_components == 1)
      format = GL_RED;
    else if (nr_components == 3)
      format = GL_RGB;
    else if (nr_components == 4)
      format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#endif

    stbi_image_free(data);
  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }

  return texture_id;
}
#endif
