#include "clib.h"

STATUS cl_array_init_size(cl_array *cl_arr, int element_size, int size) {
	void *data;
	if ((data = calloc(element_size, size)) == NULL) {
		printf("Could not initialize array\n");
		return MEMORY;
	}
	
	*cl_arr = (cl_array){
		.data = data,
		.count = 0,
		.size = size,
		.element_size = element_size,
	};

	return SUCCESS;
}

STATUS cl_array_init(cl_array *arr, int element_size) {
	return cl_array_init_size(arr, element_size, CL_ARRAY_SIZE);
}

cl_array* cl_array_create(int element_size) {
	cl_array* cl_arr = (cl_array*)calloc(sizeof(cl_array), 1);
	if (cl_array_init(cl_arr, element_size) != SUCCESS) {
		return NULL;
	} else {
		return cl_arr;
	}
}

STATUS cl_array_add(cl_array *cl_arr, void* element) {
	if (cl_arr->count >= cl_arr->size) {
		// Increase array size
		int new_size = cl_arr->count * 2;
		cl_arr->data = realloc(cl_arr->data, cl_arr->element_size * new_size);
		cl_arr->size = new_size;
		if (cl_arr->data == NULL) {
			printf("Could not resize array\n");
			return MEMORY;
		}
	}

	memcpy(ELEMENT_AT(cl_arr->data, cl_arr->count, cl_arr->element_size), element, cl_arr->element_size);
	cl_arr->count++;
	return SUCCESS;
}

void cl_array_get(cl_array *cl_arr, int index, void *dest) {
	if (index < 0 || index >= cl_arr->count) {
		printf("Array index out of bounds\n");
		dest = NULL;
	}
	memcpy(dest, ELEMENT_AT(cl_arr->data, index, cl_arr->element_size), cl_arr->element_size);
}


void cl_hashmap_init(cl_hashmap* map, int size) {
	size = size == -1 ? CL_HASHMAP_SIZE : size;
	map->size = size;
	map->count = 0;
	map->pairs = (cl_kv_pair**)calloc(sizeof(cl_kv_pair*), size);
}

unsigned cl_hashmap_hashcode(cl_hashmap* map, char* key) {
    unsigned code;
    for (code = 0; *key != '\0'; key++) {
        code = *key + 31 * code;
    }
    return code % (map->size);
}

int* cl_hashmap_get(cl_hashmap* map, char* v) {
	unsigned int code = cl_hashmap_hashcode(map, v);
	cl_kv_pair* pair = map->pairs[code];
	if (pair == NULL) return 0;

	while (pair != NULL) {
		if (strcmp(pair->key, v) == 0) {
			return &(pair->val);
		}

	    pair = pair->next;
	}

	return 0;
}

void cl_hashmap_add(cl_hashmap* map, char* k, int v) {
	unsigned int code = cl_hashmap_hashcode(map, k);

	cl_kv_pair* kv_pair = malloc(sizeof(cl_kv_pair));
	*kv_pair = (cl_kv_pair){.key = k, .val = v};
	
	cl_kv_pair* place = map->pairs[code];
	if (place == NULL) {
		map->pairs[code] = kv_pair;
	} else {
		while(place->next != NULL) {
			place = place->next;
		}
		place->next = kv_pair;
	}
}

void cl_hashmap_destroy(cl_hashmap* map) {
	for (int i = 0; i < map->size; i++) {
		if (map->pairs[i] != NULL) {
			cl_kv_pair* next = map->pairs[i];
			while (next != NULL) {
				cl_kv_pair* tmp = next->next;
				free(next);
				next = tmp;
			}
		}
	}
	free(map->pairs);
}
