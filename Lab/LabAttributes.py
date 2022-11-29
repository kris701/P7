ERROR_ATTRIBUTES= ['domain', 
                    'problem', 
                    'algorithm', 
                    'unexplained_errors', 
                    'error', 
                    'raw_memory',
                    'p7_settings_file']

PREDEFINED_ATTRIBUTES= ['cost', 
                        'coverage', 
                        'dead_ends', 
                        'evaluations', 
                        'expansions', 
                        'generated', 
                        'initial_h_value', 
                        'plan_length', 
                        'planner_time', 
                        'search_time', 
                        'translator_facts',
                        'reformulation_time',
                        'reformulated_plan_valid',
                        'rebuild_plan_valid',
                        'solvable',
                        'plan_length_difference',
                        'macros_used',
                        'unique_macros_used',
                        'macros_generated',
                        'total_walker_paths',
                        'total_walker_actions_generated',
                        'walker_average_branching_factor',
                        'walker_median_branching_factor',
                        'walker_average_path_length',
                        'walker_median_path_length'
                        ]

ATTRIBUTES = ERROR_ATTRIBUTES + PREDEFINED_ATTRIBUTES