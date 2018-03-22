#pragma once
#include "BowPrerequisites.h"
#include "BowResourcesPredeclares.h"

#include "BowMath.h"
#include "BowResource.h"

namespace bow {

	class Material
	{
		friend class MaterialCollection;
		friend class ModelLoader_obj;

	public:
		Material() : m_parent(0) {
			name = "";
			ambient_texname = "";
			diffuse_texname = "";
			specular_texname = "";
			specular_highlight_texname = "";
			bump_texname = "";
			displacement_texname = "";
			reflection_texname = "";
			alpha_texname = "";

			for (int i = 0; i < 3; i++) {
				ambient[i] = 0.f;
				diffuse[i] = 0.f;
				specular[i] = 0.f;
				transmittance[i] = 0.f;
				emission[i] = 0.f;
			}
			illum = 0;
			dissolve = 1.f;
			shininess = 1.f;
			ior = 1.f;

			roughness = 0.f;
			metallic = 0.f;
			sheen = 0.f;
			clearcoat_thickness = 0.f;
			clearcoat_roughness = 0.f;
			anisotropy_rotation = 0.f;
			anisotropy = 0.f;

			roughness_texname = "";
			metallic_texname = "";
			sheen_texname = "";
			emissive_texname = "";
			normal_texname = "";

			unknown_parameter.clear();
		}
		~Material() {}

		std::string name;

		float ambient[3];
		float diffuse[3];
		float specular[3];
		float transmittance[3];
		float emission[3];
		float shininess;
		float ior;       // index of refraction
		float dissolve;  // 1 == opaque; 0 == fully transparent
						  // illumination model (see http://www.fileformat.info/format/material/)
		int illum;

		int dummy;  // Suppress padding warning.

		std::string ambient_texname;             // map_Ka
		std::string diffuse_texname;             // map_Kd
		std::string specular_texname;            // map_Ks
		std::string specular_highlight_texname;  // map_Ns
		std::string bump_texname;                // map_bump, map_Bump, bump
		std::string displacement_texname;        // disp
		std::string alpha_texname;               // map_d
		std::string reflection_texname;          // refl

		// PBR extension
		// http://exocortex.com/blog/extending_wavefront_mtl_to_support_pbr
		float roughness;            // [0, 1] default 0
		float metallic;             // [0, 1] default 0
		float sheen;                // [0, 1] default 0
		float clearcoat_thickness;  // [0, 1] default 0
		float clearcoat_roughness;  // [0, 1] default 0
		float anisotropy;           // aniso. [0, 1] default 0
		float anisotropy_rotation;  // anisor. [0, 1] default 0

		float pad0;

		std::string roughness_texname;  // map_Pr
		std::string metallic_texname;   // map_Pm
		std::string sheen_texname;      // map_Ps
		std::string emissive_texname;   // map_Ke
		std::string normal_texname;     // norm. For normal mapping.

		int pad2;

		std::map<std::string, std::string> unknown_parameter;

	private:
		/// Reference to parent Mesh (not a smart pointer so child does not keep parent alive).
		MaterialCollection* m_parent;
	};

	// ---------------------------------------------------------------------------
	/** @brief A material to render a mesh with.
	*/
	class MaterialCollection : public Resource
	{
		friend class Material;
		friend class ModelLoader_obj;

	public:
		MaterialCollection(ResourceManager* creator, const std::string& name, ResourceHandle handle);
		~MaterialCollection();

		Material* CreateMaterial(void);
		Material* CreateMaterial(const std::string& name);

		void NameMaterial(const std::string& name, unsigned short index);
		void UnnameMaterial(const std::string& name);


		unsigned short GetMaterialIndex(const std::string& name) const;

		std::vector<Material*> GetMaterials() const {
			return m_materialList;
		}
		Material* GetMaterial(size_t index) const {
			return m_materialList[index];
		}
		Material* GetMaterial(const std::string& name) const;

		void DestroyMaterial(unsigned short index);

	private:
		/** Loads the material from disk. This call only performs IO, it
			does not parse the bytestream or check for any errors therein.
			It also does not set up submaterials, etc.  You have to call load()
			to do that.
		*/
		void VPrepareImpl(void);

		/** Destroys data cached by prepareImpl.
		*/
		void VUnprepareImpl(void);

		/// @copydoc Resource::VLoadImpl
		void VLoadImpl(void);

		/// @copydoc Resource::VPostLoadImpl
		void VPostLoadImpl(void);

		/// @copydoc Resource::VUnloadImpl
		void VUnloadImpl(void);

		char* m_dataFromDisk;

		std::vector<Material*>	m_materialList;
		std::map<std::string, unsigned short>	m_materialNameMap;
	};
}
