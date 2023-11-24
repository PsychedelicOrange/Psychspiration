import sys
print(sys.exec_prefix)
import bpy, os
from bpy_extras.io_utils import axis_conversion
from mathutils import Matrix
import mathutils
from subprocess import Popen, CREATE_NEW_CONSOLE,PIPE,STDOUT
import json
import zstandard as zstd
from collections import defaultdict
import random

cctx = zstd.ZstdCompressor(threads=-1)
#config
psych_root = r'C:\Data\psych_test\Psychspiration'
psych_executable = r'C:\Data\psych_test\Psychspiration\build\Debug\Psychspiration.exe'
#settings

# General settings

OVERWRITE_ALL_OBJECTS = True # overwrites all objects. Only use when no texture changes. Texture changes (if any) are only reflected in uncompressed image.
OVERWRITE_TEXTURES = False # overwrites Textures of objects, if objects are overwritten
INDENT_SCENE_JSON = False # Indent scene json (does'nt work) for debug
# Texture settings

GPU_COMPRESS = True # converts original textures to dds format.
ZSTD_COMPRESS = True # use zstd to compress textures.
ZSTD_COMPRESS = GPU_COMPRESS and ZSTD_COMPRESS
#--------------------------------------------------------------------------------------------------------------------------------------
# Globals
# Paths
blendname = bpy.path.basename(bpy.context.blend_data.filepath)
blendname = blendname[:blendname.rfind('.')]

DynamicObjects = bpy.data.collections["Dynamic"]
rootPath = os.path.abspath (bpy.context.scene.render.filepath)
ModelPath = rootPath +r'/Models'
ScenePath = rootPath + r'/Scenes'
TexturePath = rootPath+r'/Textures'

scene_filename = os.path.join (ScenePath,blendname +'.scene')
model_filename = os.path.join (ModelPath, '.ignore')

# data.scene
data = {
    'objects':[

        ],
    'hulls':[
        
    ],
    'lights':[

        ]
    
    }
#--------------------------------------------------------------------------------------------------------------------------------------
# Functions

# opens batch script to convert textures to dds and compress to zstd
def compressTextures(name):
    name = name.replace('%20',' ')
    name_wo_extension = name[:name.find('.')]
    if(OVERWRITE_TEXTURES): 
        print("Overwriting Texture:" + name)
    if(os.path.exists(TexturePath+"/"+name_wo_extension+'.dds') and not OVERWRITE_TEXTURES):
        print("Texture \"" + "{}".format(name) + "\" already exists, skipping gpu compress!")
    else:
        print(psych_root + r'\scripts\Blender_scripts\to_dds.bat' + ' cd '+TexturePath+ ' 'r'/'+name)
        p = Popen([psych_root + r'\scripts\Blender_scripts\to_dds.bat',TexturePath,r'/'+name],creationflags=CREATE_NEW_CONSOLE)
        p.wait()
        
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
    
def run_vhacd(name):
    script = (r'C:\Users\parth\source\repos\Psychspiration\Blender_scripts\TestVHACD.exe "{}" -o obj'.format(name))
    vhacd = Popen(script,creationflags=CREATE_NEW_CONSOLE)
    vhacd.wait()
    
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

def ResolveTextureNameConflicts():
    scene = bpy.context.scene
    for ob in scene.objects:
        filePath = ModelPath + '\\' + "{}".format(ob.name) + '.gltf'
        if not os.path.exists(filePath): # unexported object
            for mat_slot in ob.material_slots:
                    if mat_slot.material:
                        if mat_slot.material.node_tree:
                           for x in mat_slot.material.node_tree.nodes:
                                if x.type=='TEX_IMAGE':
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
    path = ModelPath + '\\' +object.name 
    print(path)
    fileExists = os.path.exists(path+'.gltf')
    if(fileExists and not OVERWRITE_ALL_OBJECTS):
            print("Object \""+ "{}".format(object.name) +"\" exists, skipping!")
    else:
        object.select_set(True)
        bpy.ops.export_scene.gltf(filepath=path,use_selection=True,export_format='GLTF_SEPARATE',export_colors=False,export_tangents=True,export_texture_dir = '..\Textures')
        object.select_set(False)

# Compress Textures according to settings, mentioned in "path" ( path to a gltf file )
def process_textures(theObject):
    path = ModelPath + '\\' + "{}".format(theObject.name) 
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

#--------------------------------------------------------------------------------------------------------------------------------------
##### START #####

# Innit
resourceFolderInit()

# Resolve texture name conflicts for unexported models
ResolveTextureNameConflicts()

# loop through all the objects in the scene
scene = bpy.context.scene
data_links = defaultdict(list)
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
#        # appending hulls
#        if hulls_exist:
#            array = []
#            for child in theObject.children:
#                array.append(child.name)
#            data['hulls'].append({theObject.name: array})
# Dump data
dumpSceneFile()
