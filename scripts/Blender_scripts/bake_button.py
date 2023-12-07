import bpy

#forward declare variables
selected_objects = []

#============= Helper Functions =============

def tex_node_exists(name,nodes):
    for tn in nodes:
        if tn.type == "TEX_IMAGE" and tn.image is not None and tn.image.name == name:
            print('Found texture node ' + name  + ' -> ' + tn.image.name)
            print(tn)
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
        deselectAllNodes(nodes)
        tex_node_found = tex_node_exists(out_name,nodes)
        if tex_node_found is not None:
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
    bpy.ops.uv.lightmap_pack(PREF_CONTEXT="ALL_FACES")#,PREF_MARGIN_DIV=18.0/1024)
    obj.select_set(False)
        
def bake(obj):
    obj.select_set(True)
    bpy.ops.object.bake(type="COMBINED",pass_filter={"INDIRECT","DIFFUSE","GLOSSY"},margin=8,use_selected_to_active = False,save_mode = "EXTERNAL", filepath = '/tmp/'+obj.name+'_baked')
    img = bpy.data.images[obj.name+'_baked']
    img.filepath_raw = '/tmp/'+obj.name+'_baked.png'
    img.file_format = 'PNG'
    img.save()
    bpy.data.images.remove(img)
    obj.select_set(False)

#============================= RUN ====================
selected_objects = bpy.context.selected_objects
deselectAllObjects()
bpy.context.scene.render.engine = 'CYCLES'
bpy.context.scene.cycles.device = 'GPU'
bpy.context.scene.cycles.samples = 10
for obj in selected_objects:    
    print(obj)
    create_or_select_UVs(obj)
    create_or_select_textures(obj)
    bake(obj)
    for mat_slot in obj.material_slots:
        nodes = mat_slot.material.node_tree.nodes
        for tn in nodes:
            if tn.type == "TEX_IMAGE" and tn.image is None:
                nodes.remove(tn)
