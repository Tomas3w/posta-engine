
def log(text):
    pass
    #with open(r'C:\Users\tomas\Documents\guardable\log_blender.txt', 'a') as logger:
    #    logger.write(text + '\n')

def get_filename_no_ext(filename):
    last_dot = filename.rfind('.')
    if last_dot == -1:
        return filename
    return filename[:last_dot]

import bpy, bmesh, sys, mathutils, itertools, math

context = bpy.context
def prant(*args, **kwargs):
    #file = open(filename, "a")
    for a in context.screen.areas:
        if a.type == 'CONSOLE':
            c = {}
            c['area'] = a
            c['space_data'] = a.spaces.active
            c['region'] = a.regions[-1]
            c['window'] = context.window
            c['screen'] = context.screen
            s = " ".join([str(arg) for arg in args])
            for line in s.split("\n"):
                bpy.ops.console.scrollback_append(c, text=line)
                #file.write(line + "\n")

# Returns the index of the bone
def get_index_from_bone_name(name):
    for i, pb in enumerate(bpy.context.active_object.parent.pose.bones):
        if pb.name == name:
            return i

# The function expects a mesh (with an armature) to be selected
# Saves in path a .bones file with the information about each vertex bone influences, starting from vertex 0
def save_bones_file(path):
    with open(path, "w") as file:
        ## create the dict for the groups attache the index to the name
        obj = bpy.context.active_object

        # get the names of the bones in the armature associated with the object
        for modifier in obj.modifiers:
            if modifier.type == 'ARMATURE':
                armature = modifier.object.data
                armature_bones = {bone.name for bone in armature.bones}
                break
            
        # get the mapping from group indices to bones
        group_to_bone = {i: group.name for i, group in enumerate(obj.vertex_groups)}
        
        # determine the bone weights associated with each vertex
        mesh = obj.data
        for vertex in mesh.vertices: ##in each vertex of the mesh
            file.write(str(vertex.index) + " ")
            for group in vertex.groups: #first loop to calculate the total weight and the space allowed if some are locked
                group_index = group.group
                group_bone = group_to_bone[group_index]
                if group_bone in armature_bones: ##if it's a bone
                    file.write(str(get_index_from_bone_name(group_bone)) + "|" + str(group.weight) + ",")
                #else:
                #    prant('    ', 'NOT A BONE:', group_bone, group.weight)
            file.write('\n')

def first_frame():
    bpy.context.scene.frame_set(0)

def set_frame(frame):
    bpy.context.scene.frame_set(frame)

def next_frame(sample_rate):
    bpy.context.scene.frame_set(min(bpy.context.scene.frame_current + sample_rate, bpy.context.scene.frame_end))

# Transforms blender axes to opengl axes
def to_opengl_axes(element):
    if isinstance(element, mathutils.Vector):
        vector = element
        return mathutils.Vector((vector.x, vector.z, -vector.y))
    elif isinstance(element, mathutils.Quaternion):
        quate = element
        quate.rotate(mathutils.Euler((-math.pi / 2, 0, 0)))
        return quate
        #eula = quate.to_euler('XYZ')
        #return mathutils.Euler((eula.x, eula.z, -eula.y), 'XYZ').to_quaternion()
    raise TypeError()

def to_opengl_scale_axes(vector):
    return mathutils.Vector((vector.x, vector.z, vector.y))

# The function expects an armature to be selected
# Saves in a .skl file the skeleton structure along with its rest transforms
# It also saves the animations in the same directory (argument path) as .anm files
def save_skeleton_and_animations(path, skeleton_filename, sample_rate):
    def get_skeleton_pose_data(obj):
        bones_transforms = []
        for pb in obj.pose.bones:
            loc = mathutils.Vector(pb.matrix.to_translation())
            rot = mathutils.Quaternion(pb.matrix.to_quaternion())
            scl = pb.matrix.to_scale()
            #loc = obj.matrix_world @ mathutils.Vector(pb.matrix.to_translation())
            #rot = mathutils.Quaternion(pb.matrix.to_quaternion())
            #scl = pb.matrix.to_scale()
            bones_transforms.append((loc, rot, scl, (pb.head - pb.tail).length))
        return bones_transforms

    def get_animation(obj, nla_track, sample_rate):
        frames = []
        set_frame(int(nla_track.strips[0].frame_start))
        for i in range((int(nla_track.strips[0].frame_end) - int(nla_track.strips[0].frame_start) + 1) // sample_rate):
            frames.append(get_skeleton_pose_data(obj))
            # jumping to next frame of animation
            next_frame(sample_rate)
        
        return frames
    
    obj = bpy.context.active_object
    bpy.ops.object.mode_set(mode='POSE') # sets pose mode if not set already
    for pb in obj.pose.bones:
        pb.bone.select = True
    bpy.ops.pose.transforms_clear()

    # saving skeleton structure
    with open(path + "/" + skeleton_filename, "w") as file:
        for pb, bone in zip(obj.pose.bones, get_skeleton_pose_data(obj)):
            file.write(f"{pb.name} {pb.parent_recursive[0].name if pb.parent_recursive else ''} {bone[3]}\n")
    # saving skeleton rest pose data
    with open(path + "/rest.anmr", "w") as file:
        for bone in get_skeleton_pose_data(obj):
            location = to_opengl_axes(bone[0])
            rotation = to_opengl_axes(bone[1])
            scale = to_opengl_scale_axes(bone[2])
            file.write(" ".join([str(x) for x in itertools.chain(location, rotation, scale)]) + "\n")
    if obj.animation_data != None:
        # for each animation record each bone location and rotation
        final_animations = {}
        for animation in obj.animation_data.nla_tracks:
            if len(animation.strips) != 0:
                # selecting animation
                for anim in obj.animation_data.nla_tracks:
                    anim.mute = not anim == animation
                # reading pose data
                final_animations[animation.name] = get_animation(obj, animation, sample_rate)
        #prant(final_animations.keys())

        # saving animations
        for anim in final_animations:
            with open(path + "/" + anim + ".anm", "w") as file:
                file.write(str(bpy.context.scene.render.fps) + " # fps\n")
                file.write(str(len(final_animations[anim])) + " # frames\n")
                file.write(str(sample_rate) + " # sample rate\n")
                file.write(str(len(final_animations[anim][0])) + " # bones\n")
                #prant("voy!")
                for frame in final_animations[anim]:
                    for bone in frame:
                        #prant(bone)
                        location = to_opengl_axes(bone[0])
                        rotation = to_opengl_axes(bone[1])
                        scale = to_opengl_scale_axes(bone[2])
                        file.write(" ".join([str(x) for x in itertools.chain(location, rotation, scale)]) + "\n")

# Addon part

bl_info = {
    "name": "Export Posta Data",
    "author": "Tomás Ayala",
    "version": (1, 0),
    "blender": (2, 80, 0),
    "location": "View3D > Export",
    "description": "This Blender addon facilitates the export of .obj and animation files optimized for seamless integration with the Posta engine",
    "warning": "",
    "doc_url": "",
    "category": "Export",
}

def write_some_data2(context, filepath):    
    obj_path = filepath + "/../" + bpy.context.active_object.name + ".obj"
    # writes the obj data
    write_some_data(context, obj_path)
    bones_path = filepath + "/../" + bpy.context.active_object.name + ".bones"
    # writes the bones data
    save_bones_file(bones_path)

    # search for the first armature modifier and select the armature object within
    obj = bpy.context.active_object
    for modifier in bpy.context.active_object.modifiers:
        if type(modifier) == bpy.types.ArmatureModifier:
            bpy.ops.object.select_all(action='DESELECT')
            bpy.context.view_layer.objects.active = modifier.object
            save_skeleton_and_animations(filepath + "/../", "skeleton.skl", 1)
    bpy.context.view_layer.objects.active = obj
    print(obj_path, bones_path)

    
    return {'FINISHED'}

def write_some_data(context, filepath):
    me = bpy.context.active_object.data
    # Get a BMesh representation
    bm = bmesh.new()
    bm.from_mesh(me)
    bmesh.ops.triangulate(bm, faces=bm.faces[:])
    bm.verts.ensure_lookup_table()
    bm.faces.ensure_lookup_table()
    
    with open(filepath, 'w', encoding='utf-8') as file:
        for vert in bm.verts:
            opengl_vert = to_opengl_axes(mathutils.Vector((vert.co.x, vert.co.y, vert.co.z)))
            file.write(f"v {opengl_vert.x} {opengl_vert.y} {opengl_vert.z}\n")
        uv_layer = bm.loops.layers.uv.active
        uvs = set()
        if uv_layer:
            for vert in bm.verts:
                uv = vert.link_loops[0][uv_layer].uv
                uvs.add(tuple(uv))
        for uv in uvs:
            file.write(f"vt {uv[0]} {uv[1]}\n")
        uvs = list(uvs)

        normals = set()
        for face in bm.faces:
            normals.add(tuple(face.normal))
        for normal in normals:
            opengl_norm = to_opengl_axes(mathutils.Vector(normal))
            file.write(f"vn {opengl_norm[0]} {opengl_norm[1]} {opengl_norm[2]}\n")
        normals = list(normals)

        for face in bm.faces:
            file.write("f")
            vn = normals.index(tuple(face.normal)) + 1
            for vert in face.verts:
                v  = ""
                vt = ""
                v = vert.index + 1
                if uvs:
                    vt = uvs.index(tuple(vert.link_loops[0][uv_layer].uv)) + 1
                file.write(f" {v}/{vt}/{vn}")
            file.write("\n")
        
    bm.free()

    return {'FINISHED'}


# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator

# Obj
class ExportPostaObj(Operator, ExportHelper):
    """Exports an obj to be used in a posta engine application"""
    bl_idname = "posta_export.obj"  # important since its how bpy.ops.import_test.some_data is constructed
    bl_label = "Export Posta Obj"

    # ExportHelper mixin class uses this
    filename_ext = ".obj"

    filter_glob: StringProperty(
        default="*.obj",
        options={'HIDDEN'},
        maxlen=255,  # Max internal buffer length, longer would be clamped.
    )
    def invoke(self, context, event):
        if not context.active_object or not context.active_object in bpy.context.selected_objects:
            self.report({'ERROR'}, "No object selected. Please select an object.")
            return {'CANCELLED'}
        if context.active_object.type != 'MESH':
            self.report({'ERROR'}, "Selected object must have a mesh")
            return {'CANCELLED'}
        log('1')
        context.window_manager.fileselect_add(self)
        log('2')
        basename = bpy.path.basename(bpy.data.filepath)
        log('2.1 ' + basename)
        xd = get_filename_no_ext(basename) + ".obj"
        log('2.2')
        self.filepath = xd
        log('3')
        return {'RUNNING_MODAL'}

    def execute(self, context):
        log('4')
        return write_some_data(context, self.filepath)

def menu_func_export(self, context):
    self.layout.operator(ExportPostaObj.bl_idname, text="Posta model (.obj)")

# Bones and animations
class ExportPostaAnimations(Operator, ExportHelper):
    """Exports animation data files to be used in a posta engine application"""
    bl_idname = "posta_export.animation"
    bl_label = "Export Posta Animation Data"

    # ExportHelper mixin class uses this
    filename_ext = ".skl"
    filename = "skeleton"

    filter_glob: StringProperty(
        default="*.skl",
        options={'HIDDEN'},
        maxlen=255,  # Max internal buffer length, longer would be clamped.
    )
    def invoke(self, context, event):
        if not context.active_object or not context.active_object in bpy.context.selected_objects:
            self.report({'ERROR'}, "No object selected. Please select an object.")
            return {'CANCELLED'}
        if context.active_object.type != 'MESH':
            self.report({'ERROR'}, "Selected object must have a mesh")
            return {'CANCELLED'}
        # Searching for the first armature modifier
        found_armature_modifier = False
        for modifier in bpy.context.active_object.modifiers:
            if type(modifier) == bpy.types.ArmatureModifier:
                found_armature_modifier = True
                break
        if not found_armature_modifier:
            self.report({'ERROR'}, "Selected object must have an armature modifier")
            return {'CANCELLED'}
        context.window_manager.fileselect_add(self)
        self.filepath = "skeleton.skl"
        return {'RUNNING_MODAL'}

    def execute(self, context):
        return write_some_data2(context, self.filepath)
def menu_func_export2(self, context):
    self.layout.operator(ExportPostaAnimations.bl_idname, text="Posta animation data (.bones, .skl, .anm, .anmr)")

def register():
    bpy.utils.register_class(ExportPostaObj)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)
    bpy.utils.register_class(ExportPostaAnimations)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export2)


def unregister():
    bpy.utils.unregister_class(ExportPostaObj)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)
    bpy.utils.unregister_class(ExportPostaAnimations)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export2)


if __name__ == "__main__":
    register()

    # test call
    #bpy.ops.export_test.some_data('INVOKE_DEFAULT')
