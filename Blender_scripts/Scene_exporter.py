import bpy, os
from bpy_extras.io_utils import axis_conversion
from mathutils import Matrix
import mathutils
from subprocess import Popen, CREATE_NEW_CONSOLE,PIPE,STDOUT
import json
#globals
    # get path to render output (usually /tmp\)
rootPath = os.path.abspath (bpy.context.scene.render.filepath)
ModelPath = rootPath +r'/Models'
ScenePath = rootPath + r'/Scenes'
TexturePath = rootPath+r'/Textures'
#settings
export_models = True
export_models_metadata = True
compress_textures = True # make sure texture  file names do not contain 'jpg' or 'png' in filename ( for compressed )
scene_metadata = True
keep_both_textures = False # dont delete uncompressed textures and keep a separate gltf for uncompressed
#functions
def get_export_name(dup_name):
    rfound = dup_name.rfind('.')
    if rfound==-1:
        return dup_name   
    else:
        return dup_name[0:rfound]
def to_dds(name1,name2): # opens batch script to convert textures to dds 
    _name1 = name1[name1.rfind('/'):]
    _name2 = name2[name2.rfind('/'):]
    print(r'C:\Users\parth\source\repos\Psychspiration\Blender_scripts\to_dds.bat'+TexturePath+' '+_name1+' '+_name2)
    if(not os.path.exists(TexturePath+_name2)):
        p = Popen([r'C:\Users\parth\source\repos\Psychspiration\Blender_scripts\to_dds.bat',TexturePath,_name1,_name2],creationflags=CREATE_NEW_CONSOLE)
        p.wait()
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
            name_initial = i['uri']
            i['uri'] = i['uri'].replace('jpg','dds')
            i['uri']= i['uri'].replace('png','dds')
            name_after = i['uri']
            #print(name_after)
            to_dds(name_initial,name_after)
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
    
# get the current selection
selection = bpy.context.selected_objects
# initialize a blank result variable
result = ""
result_lights=""
# make a filename
blendname = bpy.path.basename(bpy.context.blend_data.filepath)
blendname = blendname[:blendname.rfind('.')]
filename = os.path.join (ScenePath,blendname +'.csv')
filename2 = os.path.join (ScenePath,blendname+'_lights.csv')
filename3 = os.path.join (ModelPath, '.ignore')
# confirm path exists
os.makedirs(os.path.dirname(filename), exist_ok=True)
os.makedirs(os.path.dirname(filename2), exist_ok=True)
os.makedirs(os.path.dirname(filename3), exist_ok=True)
bpy.ops.object.select_all(action='DESELECT')    
# loop through all the objects in the scene
scene = bpy.context.scene

for ob in scene.objects:
    # make the current object active and select it
    bpy.context.view_layer.objects.active = ob
    ob.select_set(True)
    
    obj = bpy.context.object
    # make sure that we only export meshes
    if ob.type == 'MESH':
        # export the currently selected object to its own file based on its name
        ob.rotation_mode = 'AXIS_ANGLE'
        ob.rotation_mode = 'XYZ'
        export_name = get_export_name(ob.name)
        filepath_object=bpy.context.scene.render.filepath+'\Models\\'+"{}".format(export_name)
        print(filepath_object+'.gltf')
        print(os.path.exists(filepath_object+'.gltf'))
        if(export_models and not os.path.exists(filepath_object+'.gltf')):
            bpy.ops.export_scene.gltf(filepath=filepath_object,use_selection=True,export_format='GLTF_SEPARATE',export_colors=False,export_tangents=True,export_texture_dir = '..\Textures')
            if(compress_textures):
                editglTF(export_name)
        # write the selected object's name and dimensions to a string
        result += ob.name
        result += ","
        result += export_name
        result += ","
        if(ob.name[0] == '_'): # Names starting with _ are convex hulls 
            for i in get_opengl(ob.matrix_local):
                for j in i:
                    result += "{}".format(round(j,5))
                    result += ","
        else: 
            for i in get_opengl(ob.matrix_world):
                for j in i:
                    result += "{}".format(round(j,5))
                    result += ","
        ob.select_set(False)
    # deselect the object and move on to another if any more are left
    if obj.type == 'LIGHT':
            print("light detect")
            loc = obj.location
            power = obj.data.energy
            color = obj.data.color
            use_shadows = obj.data.use_shadow
            range = obj.data.cutoff_distance
            result_lights+=obj.data.name
            result_lights += ","
            for i in loc:
               result_lights += "{}".format(round(i,5))
               result_lights += ","
            for i in color:
                result_lights += "{}".format(round(i,5))
                result_lights += ","
            result_lights +="{}".format(round(power,5))
            result_lights +=","
            result_lights +="{}".format(round(range,5))
            result_lights +=","
            result_lights +="{}".format(round(use_shadows,5))
            result_lights +=","
            ob.select_set(False)

if(export_models_metadata):    
    # open a file to write to
    file = open(filename, "w")
    # write the data to file
    file.write(result)
    # close the file
    file.close()
# writing lights
file2 = open(filename2,"w")
file2.write(result_lights)
file2.close()
