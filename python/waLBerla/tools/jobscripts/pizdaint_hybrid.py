from __future__ import print_function, absolute_import, division, unicode_literals

import os
import math



def createJobscript( wall_time = None, nodes = None, cores = None, initial_dir=None, job_name="waLBerla",
                     exe_name=None, parameter_files=[], commands=[], hyperthreading=1, **kwargs ):
    if type(hyperthreading) is bool:
        hyperthreading = 2 if hyperthreading else 1

    CORES_PER_NODE = 12 * hyperthreading

    if wall_time and wall_time.total_seconds() >  24 * 3600:
        raise ValueError("No jobs longer that 24h allowed")

    if hyperthreading > 2:
        raise ValueError("PizDaint supports only two way hyperthreading (requested %d)" %(hyperthreading,) )

    if nodes is not None and cores is not None:
        raise ValueError("You can either specify nodes or cores - not both.")

    if nodes is None and cores is None:
        raise ValueError('Specify either cores or nodes.')

    if nodes is None:
        nodes = math.ceil( cores / CORES_PER_NODE )
    if cores is None:
        cores = nodes * CORES_PER_NODE

    if cores > CORES_PER_NODE and cores % CORES_PER_NODE != 0:
        raise ValueError("When using more than one node, the number of cores has to be a multiple of 12")

    tasks_per_node = min( CORES_PER_NODE, cores )

    template_file = os.path.join(  os.path.dirname( os.path.realpath(__file__)  ), "pizdaint_hybrid.job" )
    result = open(template_file).read().format( cores = cores,
                                                nodes = nodes,
                                                tasks_per_core = hyperthreading,
                                                tasks_per_node=tasks_per_node,
                                                cpus_per_task = 1,  # OpenMP num threads would go here
                                                initial_dir = initial_dir,
                                                job_name = job_name,
                                                wall_time = wall_time )

    exec_line = "srun %s %s \n"

    if exe_name is not None:
        for param_file in parameter_files:
            result += exec_line %( cores, exe_name, param_file )

    for exe_paramfile_pair in commands:
        if type(exe_paramfile_pair) is not tuple:
            result += exe_paramfile_pair + "\n"
        else:
            result += exec_line % exe_paramfile_pair


    return result

