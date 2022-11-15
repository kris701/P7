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
                        'rebuild_plan_length',
                        'reformulated_plan_length',
                        'macros_used']

ATTRIBUTES = ERROR_ATTRIBUTES + PREDEFINED_ATTRIBUTES