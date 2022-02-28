import bpy, os
from bpy_extras.io_utils import axis_conversion
from mathutils import Matrix
import mathutils

# get the current selection
selection = bpy.context.selected_objects
# axis conversion

def get_opengl(mat):
    global_matrix = axis_conversion(to_forward="-Z", to_up="Y").to_4x4()
    return (global_matrix@ mat @ global_matrix.inverted()).transposed()
# initialize a blank result variable
result = ""
result_lights=""
# get path to render output (usually /tmp\)
tempFolder = os.path.abspath (bpy.context.scene.render.filepath)
# make a filename
filename = os.path.join (tempFolder, "scene_prop.csv")
filename2 = os.path.join (tempFolder, "scene_lights.csv")
# confirm path exists
os.makedirs(os.path.dirname(filename), exist_ok=True)
os.makedirs(os.path.dirname(filename2), exist_ok=True)
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
        
        bpy.ops.export_scene.gltf(filepath=bpy.context.scene.render.filepath+"{}".format(ob.name),use_selection=True,export_format='GLB',export_colors=False,export_tangents=True)

        #transform = matrix_world.transposed()
        #transform_hull = matrix_local.transposed()
        # write the selected object's name and dimensions to a string
        result += ob.name
        result += ","
        if(ob.name[0] == '_'):
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