import bpy, sys, mathutils, itertools, math

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
        return mathutils.Vector((vector.x, vector.z, -vector.y)) # DEBUG
    elif isinstance(element, mathutils.Quaternion):
        quate = element
        quate.rotate(mathutils.Euler((-math.pi / 2, 0, 0)))
        return quate
        #eula = quate.to_euler('XYZ')
        #return mathutils.Euler((eula.x, eula.z, -eula.y), 'XYZ').to_quaternion()
    raise TypeError()

# The function expects an armature to be selected
# Saves in a .skl file the skeleton structure along with its rest transforms
# It also saves the animations in the same directory (argument path) as .anm files
def save_skeleton_and_animations(path, skeleton_filename, sample_rate):
    def get_skeleton_pose_data(obj):
        bones_transforms = []
        for pb in obj.pose.bones:
            #loc = obj.matrix_world @ mathutils.Vector(pb.head)
            loc = obj.matrix_world @ mathutils.Vector(pb.matrix.to_translation())
            rot = mathutils.Quaternion(pb.matrix.to_quaternion())
            bones_transforms.append((loc, rot, (pb.head - pb.tail).length))
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
            file.write(f"{pb.name} {pb.parent_recursive[0].name if pb.parent_recursive else ''} {bone[2]}\n")
    # saving skeleton rest pose data
    with open(path + "/rest.anmr", "w") as file:
        for bone in get_skeleton_pose_data(obj):
            location = to_opengl_axes(bone[0])
            rotation = to_opengl_axes(bone[1])
            file.write(" ".join([str(x) for x in itertools.chain(location, rotation)]) + "\n")
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
                        file.write(" ".join([str(x) for x in itertools.chain(location, rotation)]) + "\n")

# The function expects a mesh to be selected
def save_obj(path):
    bpy.ops.export_scene.obj(filepath=path, use_materials=False, use_triangles=True, use_selection=True, axis_forward='-Z', axis_up='Y', use_mesh_modifiers=False)
    #bpy.ops.export_scene.obj(filepath=path, use_materials=False, use_triangles=True, use_selection=True, axis_forward='Y', axis_up='Z', use_mesh_modifiers=False) # DEBUG

assets_directory = "C:\\posta-engine\\apps\\shadow_test\\assets\\unidad\\"
base_directory = assets_directory + "mago/"

#save_obj(base_directory + "mago.obj")

#save_bones_file(base_directory + "mago.bones")
save_skeleton_and_animations(base_directory, "skeleton.skl", 1)


