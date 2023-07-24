import sys
print(sys.exec_prefix)
import bpy, os
from bpy_extras.io_utils import axis_conversion
from mathutils import Matrix
import mathutils
from subprocess import Popen, CREATE_NEW_CONSOLE,PIPE,STDOUT
import json
import zstandard as zstd
cctx = zstd.ZstdCompressor(threads=-1)
#config
psych_root = r'C:\Data\psych_test\Psychspiration'
psych_executable = r'C:\Data\psych_test\Psychspiration\build\Debug\Psychspiration.exe'
#settings
export_hulls = True
export_models_metadata = True
compress_textures = True # make sure texture  file names do not contain 'jpg' or 'png' in filename ( for compressed )
scene_metadata = True
use_zstd = True 
use_zstd = use_zstd and compress_textures
keep_both_textures = False # dont delete uncompressed textures and keep a separate gltf for uncompressed
#globals

rootPath = os.path.abspath (bpy.context.scene.render.filepath)
ModelPath = rootPath +r'/Models'
HullPath = rootPath + r'/Hulls'
ScenePath = rootPath + r'/Scenes'
TexturePath = rootPath+r'/Textures'
data = {
    'objects':[

        ],
    'lights':[

        ]
    
    }
#functions
def get_export_name(dup_name):
    rfound = dup_name.rfind('.')
    if rfound==-1:
        return dup_name   
    else:
        return dup_name[0:rfound]
def compressTextures(name): # opens batch script to convert textures to dds and compress to zstd
    name = name[:name.find('.')]
    print('texture_name: '+name)
    if(not os.path.exists(TexturePath+name+'.dds')):
        print(psych_root + r'\scripts\Blender_scripts\to_dds.bat' + ' cd '+TexturePath+ ' 'r'/'+name+'.png'+' '+r'/'+name+'.dds')
        p = Popen([psych_root + r'\scripts\Blender_scripts\to_dds.bat',TexturePath,r'/'+name+'.png',r'/'+name+'.dds'],creationflags=CREATE_NEW_CONSOLE)
        q = Popen([psych_root + r'\scripts\Blender_scripts\to_dds.bat',TexturePath,r'/'+name+'.jpg',r'/'+name+'.dds'],creationflags=CREATE_NEW_CONSOLE)
        p.wait()
        q.wait()
        if(use_zstd and not os.path.exists(TexturePath+name+'.zstd')):
            to_zstd(name)
def to_zstd(name): # compresses dds textures to zstd
    TextureUncompressedFile = open(os.path.join(TexturePath,name+'.dds'),'rb')
    compressed = cctx.compress(TextureUncompressedFile.read())
    TextureUncompressedFile.close()
    TextureCompressedFile = open(os.path.join(TexturePath,name+'.zstd'),'wb')
    TextureCompressedFile.write(compressed)
    TextureCompressedFile.close()
    
def run_vhacd(name):
    script = (r'C:\Users\parth\source\repos\Psychspiration\Blender_scripts\TestVHACD.exe "{}" -o obj'.format(name))
    vhacd = Popen(script,creationflags=CREATE_NEW_CONSOLE)
    vhacd.wait()
    
def editglTF(name):# edit gltf to support dds textures 
    name = name + '.gltf'
    path = os.path.join (ModelPath, name)
    model = open(path,mode='r')
    model_gltf = json.load(model)
    model.close()
    if(keep_both_textures):
        with open(ob.name+'_uncompressed','w') as f:
            json.dump(model_gltf,f)
    model_gltf.update({"extensionsUsed":["MSFT_texture_dds"]})
    model_gltf.update({"extenionsRequired":["MSFT_texture_dds"]})
    print(name)
    if('images' in model_gltf):
        for i in model_gltf['images']:
            name_initial = i['uri'] # ..textures/_Tname_.png/jpg
            if(use_zstd):
                i['uri'] = i['uri'].replace('jpg','zstd')
                i['uri']= i['uri'].replace('png','zstd')
            else:
                i['uri'] = i['uri'].replace('jpg','dds')
                i['uri']= i['uri'].replace('png','dds')
            compressTextures(i['uri'][i['uri'].rfind('/')+1:]) # _Tname_.zstb 
            
            i['mimeType'].replace('jpeg','vnd-ms.dds')
            i['mimeType'].replace('png','vnd-ms.dds')
        #p = Popen(['del',r'\textures\*.png'],creationflags=CREATE_NEW_CONSOLE)
        #p = Popen(['del',r'\textures\*.jpg'],creationflags=CREATE_NEW_CONSOLE)
    if('textures' in model_gltf):
        for index,texture in enumerate(model_gltf['textures']):
            texture.update({'extensions':{'MSFT_texture_dds':{'source':index}}})
    model = open(path,'w')
    json.dump(model_gltf,model)
    model.close()
    
def get_opengl(mat): # axis conversion
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
# get the current selection
selection = bpy.context.selected_objects
# initialize a blank result variable
result = ""
result_lights=""
# make a filename
blendname = bpy.path.basename(bpy.context.blend_data.filepath)
blendname = blendname[:blendname.rfind('.')]
#filename = os.path.join (ScenePath,blendname +'.csv')
#filename2 = os.path.join (ScenePath,blendname+'_lights.csv')
filename = os.path.join (ScenePath,blendname +'.scene')
filename2 = os.path.join (HullPath, '.ignore')
filename3 = os.path.join (ModelPath, '.ignore')
# confirm path exists
os.makedirs(os.path.dirname(filename), exist_ok=True)
os.makedirs(os.path.dirname(filename2), exist_ok=True)
os.makedirs(os.path.dirname(filename3), exist_ok=True)
bpy.ops.object.select_all(action='DESELECT')
# loop through all the objects in the scene
scene = bpy.context.scene

DynamicObjects = bpy.data.collections["Dynamic"]


for ob in scene.objects:
    # make the current object active and select it
    bpy.context.view_layer.objects.active = ob
    ob.select_set(True)
    obj = bpy.context.object
    # make sure that we only export meshes
    if ob.type == 'MESH':
        # export the currently selected object to its own file based on its name
        #ob.rotation_mode = 'AXIS_ANGLE'
        ob.rotation_mode = 'XYZ'
        dynamic = False
        if DynamicObjects in ob.users_collection:
            dynamic = True
        if( not ('_hull' in ob.name) ):
            # export objects 
            export_name = get_export_name(ob.name)
            filepath_object=ModelPath +'\\'+"{}".format(export_name)
            if(not os.path.exists(filepath_object+'.gltf')):
                bpy.ops.export_scene.gltf(filepath=filepath_object,use_selection=True,export_format='GLTF_SEPARATE',export_colors=False,export_tangents=True,export_texture_dir = '..\Textures')
                if(compress_textures):
                    editglTF(export_name)
            openglMat = get_opengl(ob.matrix_world)
            
            # write objects to scene data   
            data['objects'].append({'name':ob.name,'export_name':export_name,'scale':convertvectolist(ob.scale),'translate':convertvectolist(ob.location),'rotate':convertvectolist(openglMat.to_euler("XYZ")),'transform':convertmattolist(openglMat),'dynamic':dynamic})
            # exports hulls
            for child in ob.children:
                ob.select_set(False)
                child.select_set(True)
                filepath_hull = ModelPath + r'/' + "{}".format(child.name)
                print(filepath_hull)
                if(export_hulls and not os.path.exists(filepath_hull+'.gltf')):
                    bpy.ops.export_scene.gltf(filepath=filepath_hull,use_selection=True,export_format='GLTF_EMBEDDED')
                child.select_set(False)
                ob.select_set(True)
        ob.select_set(False)
    # deselect the object and move on to another if any more are left
    if obj.type == 'LIGHT':
        print("light detect")
        data['lights'].append({'name':obj.data.name,'location':convertvectolist(obj.location),'color':convertvectolist(obj.data.color),'power':obj.data.energy,'range':obj.data.cutoff_distance,'use_shadow':obj.data.use_shadow})
        ob.select_set(False)

if(export_models_metadata):
    data['objects'].sort(key = lambda x: x['export_name'])
    json_string = json.dumps(data)
    #print(json_string)    
    # open a file to write to
    if os.path.exists(filename):
        os.remove(filename)
    file = open(filename, "w")
    # write the data to file
    file.write(json_string)
    # close the file
    file.close()
#p = Popen([psych_executable,],creationflags=CREATE_NEW_CONSOLE)
# writing lights
#file2 = open(filename2,"w")
#file2.write(result_lights)
#file2.close()
