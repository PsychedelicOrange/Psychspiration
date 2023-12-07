import sys
print(sys.exec_prefix)
import bpy, os, platform
from bpy_extras.io_utils import axis_conversion
from mathutils import Matrix
import mathutils
if(platform.system() == 'Windows'):
    from subprocess import Popen, CREATE_NEW_CONSOLE,PIPE,STDOUT
if(platform.system() == 'Linux'):
    import subprocess
import json
import zstandard as zstd
from collections import defaultdict
import random
cctx = zstd.ZstdCompressor(threads=-1)

#=============================================================================================
#======================================= Export scene ===================================
#=============================================================================================

# ================== Settings ==========================

psych_root = r'C:\Data\psych_test\Psychspiration'
psych_executable = r'C:\Data\psych_test\Psychspiration\build\Debug\Psychspiration.exe'
# General settings
OVERWRITE_ALL_OBJECTS = True # overwrites all objects. Only use when no texture changes. Texture changes (if any) are only reflected in uncompressed image.
OVERWRITE_TEXTURES = False # overwrites Textures of objects, if objects are overwritten.
INDENT_SCENE_JSON = False # Indent scene json (does'nt work) for debug.
# Texture settings
BAKE_LIGHTMAPS = False # bakes light map for each object.
GPU_COMPRESS = True # converts original textures to dds format.
ZSTD_COMPRESS = True # use zstd to compress textures.
ZSTD_COMPRESS = GPU_COMPRESS and ZSTD_COMPRESS

# ================== Global variables ==========================

DynamicObjects = bpy.data.collections["Dynamic"]
# data.scene
data = {
    'objects':[

        ],
    'hulls':[
        
    ],
    'lights':[

        ]
    
    }
# Paths
blendname = bpy.path.basename(bpy.context.blend_data.filepath)
blendname = blendname[:blendname.rfind('.')]
rootPath = os.path.abspath (bpy.context.scene.render.filepath)
ModelPath = rootPath +r'/Models'
ScenePath = rootPath + r'/Scenes'
TexturePath = rootPath+r'/Textures'
scene_filename = os.path.join (ScenePath,blendname +'.scene')
model_filename = os.path.join (ModelPath, '.ignore')
texture_filename = os.path.join (TexturePath, '.ignore')
# ================== Helper functions ==========================

 # axis conversion
def get_opengl(mat):
    global_matrix = axis_conversion(to_forward="-Z", to_up="Y").to_4x4()
    return (global_matrix@ mat @ global_matrix.inverted()).transposed()

def convertmattolist(mat):
    lmao = []
    for i in mat:
        lmaopart=[]
        for j in i:
            lmaopart.append(j)
        lmao.append(lmaopart)
    return lmao

def convertvectolist(vec):
    lmao = []
    for i in vec:
        lmao.append(i)
    return lmao

# ================== Main functions ==========================

# runs script to convert textures to dds
def compressTextures(name):
    name = name.replace('%20',' ')
    name_wo_extension = name[:name.find('.')]
    if(OVERWRITE_TEXTURES): 
        print("Overwriting Texture:" + name)
    if(os.path.exists(TexturePath+"/"+name_wo_extension+'.dds') and not OVERWRITE_TEXTURES):
        print("Texture \"" + "{}".format(name) + "\" already exists, skipping gpu compress!")
    else:
        if(platform.system() == 'Windows'):
            print(psych_root + r'\scripts\Blender_scripts\to_dds.bat' + ' cd '+TexturePath+ ' 'r'/'+name)
            p = Popen([psych_root + r'\scripts\Blender_scripts\to_dds.bat',TexturePath,r'/'+name],creationflags=CREATE_NEW_CONSOLE)
            p.wait()
        if(platform.system() == 'Linux'):
            P = subprocess.call(['sh', '/home/parth/code/Psychspiration/scripts/Blender_scripts/to_dds.sh', TexturePath+'/'+name, TexturePath])
        
# compresses dds textures to zstd
def to_zstd(name):
    name = name.replace('%20',' ')
    name_wo_extension = name[:name.find('.')]
    if(os.path.exists(TexturePath+name_wo_extension+'.zstd') and not OVERWRITE_TEXTURES):
        print("Texture \"" + "{}".format(name) + "\" already exists, skipping zstd compress!")
    print(TexturePath+'\\'+name_wo_extension+'.dds')
    if(os.path.exists(TexturePath+'/'+name_wo_extension+'.dds')):        
        name = name_wo_extension + '.dds'
    TextureUncompressedFile = open(os.path.join(TexturePath,name),'rb')
    compressed = cctx.compress(TextureUncompressedFile.read())
    TextureUncompressedFile.close()
    TextureCompressedFile = open(os.path.join(TexturePath,name_wo_extension+'.zstd'),'wb')
    TextureCompressedFile.write(compressed)
    TextureCompressedFile.close()
        
def ResolveTextureNameConflicts():
    scene = bpy.context.scene
    for ob in scene.objects:
        filePath = ModelPath + '\\' + "{}".format(ob.name) + '.gltf'
        if not os.path.exists(filePath): # unexported object
            for mat_slot in ob.material_slots:
                    if mat_slot.material:
                        if mat_slot.material.node_tree:
                           for x in mat_slot.material.node_tree.nodes:
                                if x.type=='TEX_IMAGE' and x.image is not None:
                                    if(os.path.exists(TexturePath + '\\' +str(x.image.name))): # if texture with same name exists on file
                                        choice = input("WARNING: Object \" "+ob.name+" \", has texture +" + str(x.image.name)+ " which already exists. '1' to rename object related texture, '2' share textures (don't overwrite) , '3' Abort object export")
                                        if(choice == 1):    # rename
                                            x.image.name += "_1"
                                        elif (choice == 2): # share
                                            pass 
                                        elif (choice == 3): # abort
                                            exit()
                               
# Export object to path, in gltf separate format
def exportObject(object):
    path = ModelPath + '/' +object.name 
    print("exporting object at: " + path + "...")
    fileExists = os.path.exists(path+'.gltf')
    if(fileExists and not OVERWRITE_ALL_OBJECTS):
            print("Object \""+ "{}".format(object.name) +"\" exists, skipping!")
    else:
        object.select_set(True)
        bpy.ops.export_scene.gltf(filepath=path,use_selection=True,export_format='GLTF_SEPARATE',export_colors=False,export_tangents=True,export_texture_dir = '../Textures')
        object.select_set(False)
    print("export complete.")

# Compress Textures according to settings, mentioned in "path" ( path to a gltf file )
def process_textures(theObject):
    path = ModelPath + '/' + "{}".format(theObject.name) 
    model = open(path+'.gltf',mode='r')
    model_gltf = json.load(model)
    model.close()
    if('images' in model_gltf):
        for i in model_gltf['images']:
            name_initial = i['uri'] # ..textures/_Tname_.png/jpg
            fileName = i['uri'][i['uri'].rfind('/')+1:] # _Tname_.png/jpg 
            if(GPU_COMPRESS):
                compressTextures(fileName) 
            if(ZSTD_COMPRESS):
                to_zstd(fileName)
                            

# Resource Folder Initialize
def resourceFolderInit():
    # confirm path exists
    os.makedirs(os.path.dirname(scene_filename), exist_ok=True)
    os.makedirs(os.path.dirname(model_filename), exist_ok=True)
    os.makedirs(os.path.dirname(texture_filename), exist_ok=True)
    bpy.ops.object.select_all(action='DESELECT')

def dumpSceneFile():
    data['objects'].sort(key = lambda x: x['export_name'])
    
    if os.path.exists(scene_filename):
        os.remove(scene_filename)
        
    file = open(scene_filename, "w")
    if(INDENT_SCENE_JSON):
        json.dump(data, file, indent = 4)
    else:
        json.dump(data, file)
    file.close()

# ================== RUN ==========================

# Innit
resourceFolderInit()

# Resolve texture name conflicts for unexported models
ResolveTextureNameConflicts()

# loop through all the objects in the scene
scene = bpy.context.scene
data_links = defaultdict(list)

# Exporting only unique data_links, (only one alt duplicates)
# WARNING : All objects having ob.data equal are treated as having same material/textures.
for obj in scene.objects:
    if obj.type == 'MESH':
        data_links[obj.data].append(obj)
    if obj.type == 'LIGHT':
        data['lights'].append({'name':obj.data.name,'location':convertvectolist(obj.location),'color':convertvectolist(obj.data.color),'power':obj.data.energy,'range':obj.data.cutoff_distance,'use_shadow':obj.data.use_shadow})

for mesh, objects in data_links.items():    
# for each mesh shared between multiple objects, export any one object and its hulls (if any)
    
    # pick alphabetically first object name as theObject
    objects.sort(key = lambda x: x.name)
    theObject = objects[0]   
    # export gltf for theObject
    is_hull = '_hull' in theObject.name
    hulls_exist = DynamicObjects in theObject.users_collection
    if not is_hull : # not a hull
        exportObject(theObject)
        process_textures(theObject)
        # export hulls for theObject
        if hulls_exist:
            for child in theObject.children:
                if(child.type == 'MESH'):
                    if('_hull' in child.name):
                        print(child.name)
                        exportObject(child)
                    
    # all objects now will refer to theObject's gltf file and hulls, now writing these objects to scene file
    if not is_hull:
        for object in objects:
            openglMat = get_opengl(object.matrix_world)
            object.rotation_mode = 'XYZ'
            data['objects'].append({'name':object.name,'export_name':theObject.name,'scale':convertvectolist(object.scale),'translate':convertvectolist(object.location),'rotate':convertvectolist(openglMat.to_euler("XYZ")),'transform':convertmattolist(openglMat),'dynamic':hulls_exist})

dumpSceneFile()
print("Scene export complete.")
#=============================================================================================
#======================================= Baking light maps ===================================
#=============================================================================================


#============= Helper Functions =============

def getAllMeshObjects():
    selected_objects = []
    for obj in scene.objects:
        if obj.type == 'MESH':
            selected_objects.append(obj)
    return selected_objects

def tex_node_exists(name,nodes):
    for tn in nodes:
        if tn.type == "TEX_IMAGE" and tn.image is not None and tn.image.name == name:
            print(name  + ' -> ' + tn.image.name)
            return tn
    return None
def deselectAllObjects():
    for ob in bpy.context.selected_objects:
        ob.select_set(False)
def deselectAllNodes(nodes):
    for node in nodes:
        node.select =False
        
#============= Main Functions ================

def create_or_select_textures(obj):
    # create image if doesn't exists
    out_name = obj.name + '_baked'
    if out_name not in bpy.data.images:
        out_image = bpy.data.images.new(out_name,1024,1024)
    
    # create and select texture image node if doesn't exist
    for mat_slot in obj.material_slots:
        nodes = mat_slot.material.node_tree.nodes
        tex_node_found = tex_node_exists(out_name,nodes)
        if tex_node_found is not None:
            deselectAllNodes(nodes)
            tex_node_found.select = True
        else:
            tex_node = nodes.new("ShaderNodeTexImage")
            tex_node.image = bpy.data.images[out_name]
            deselectAllNodes(nodes)
            tex_node.select = True

def create_or_select_UVs(obj):
    lightmap_uv_name = 'UVMap_Lightmap'
    if lightmap_uv_name not in obj.data.uv_layers:
        obj.data.uv_layers.new(name = lightmap_uv_name)
    obj.data.uv_layers[lightmap_uv_name].active = True
    obj.select_set(True)
    bpy.ops.uv.lightmap_pack(PREF_CONTEXT="ALL_FACES")#,PREF_MARGIN_DIV=1)
    obj.select_set(False)
        
def bake(obj):
    print("Baking for object : "+ obj.name + " ....")
    obj.select_set(True)
    bpy.ops.object.bake(type="COMBINED",pass_filter={"INDIRECT","DIFFUSE","GLOSSY"},use_selected_to_active = False)
    img = bpy.data.images[obj.name+'_baked']
    img.filepath_raw = TexturePath + '/' + obj.name + '_baked.hdr'
    img.file_format = 'HDR'
    img.save()
    obj.select_set(False)
    print("Bake complete, saved at Textures/"+obj.name+ ".")
        
def run_bake():
    bpy.context.scene.render.engine = 'CYCLES'
    bpy.context.scene.cycles.device = 'GPU'
    bpy.context.scene.cycles.samples = 206
    selected_objects = getAllMeshObjects()
    deselectAllObjects()
    for obj in selected_objects:
        create_or_select_UVs(obj)
        create_or_select_textures(obj)
        bake(obj)
        for mat_slot in obj.material_slots:
            nodes = mat_slot.material.node_tree.nodes
            for tn in nodes:
                if tn.type == "TEX_IMAGE" and tn.image is None:
                    nodes.remove(tn)

if BAKE_LIGHTMAPS:
    run_bake()