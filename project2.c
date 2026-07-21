#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Target {
	char category[20];
	double conf;
	int x;
	int y;
	int w;
	int h;
} Target;

typedef struct FrameData{
	int frame_id;
	char time_str[25];
	char cam_id[10];
	int target_count;
	Target targets[10]; 
} FrameData;

typedef struct VisionFrame {
	struct FrameData data;
	struct VisionFrame *next;
} VisionFrame;

//other functions

int get_menu();

//menu
VisionFrame *loadfile();
void add_frame();
void print_all(const VisionFrame *head);
void filter(VisionFrame **head);
void sort_frame();
void search();
void summary(const VisionFrame *head);
void check_memory(const VisionFrame *head);
void clear_all(VisionFrame **head);
void exit_it(VisionFrame **head);


int main(){
	bool loop = true;	
	VisionFrame *head = NULL;
	while(loop == true){
		int choice = get_menu();
		switch(choice){
		case 1:{head = loadfile();break;}
		case 2:{add_frame();break;}
		case 3: { print_all(head); break; 
		case 4:{ filter(&head); break; 
		case 5:{sort_frame();break;}
		case 6:{search();break;}
		case 7:{summary(head);break;}
		case 8:{check_memory(head);break;}
		case 9:{clear_all(&head);break;}
		case 0:{exit_it(&head); loop = false; break;}
		default:{printf("Invalid choice\n");}
		}

	}
	
}

//linklist tools

VisionFrame *create_node(FrameData value){
	VisionFrame *new_node = malloc(sizeof(VisionFrame));

	if (new_node == NULL){
		fprintf(stderr, "error: Memory allocation failed. \n");
		exit(EXIT_FAILURE);
	}

	new_node -> data = value;
	new_node -> next = NULL;

	return new_node;
}

int delete_node(VisionFrame **head, FrameData value){
	if (head == NULL || *head == NULL) {
		return 0;
	}

	VisionFrame *current = *head;
	VisionFrame *previous = NULL;

	while (current != NULL &&
	       current->data.frame_id != value.frame_id) {
		previous = current;
		current = current->next;
	}

	if (current == NULL) {
		return 0;
	}

	if (previous == NULL) {
		*head = current->next;
	}
	else {
		previous->next = current->next;
	}

	free(current);
	return 1;
}

void append_node(VisionFrame **head, FrameData value){
	
	VisionFrame *new_node = create_node(value);

	if (*head == NULL){
		*head = new_node;
		return;
	}
	VisionFrame *last = *head;
	while(last -> next != NULL){
		last = last -> next; 
	}

	last -> next = new_node;
}

//only print frame_id this version	
void print_list(const VisionFrame *head){
	const VisionFrame *current = head;

	while(current != NULL){
		printf("%d", current -> data.frame_id);
		if(current -> next != NULL){printf("->");}
		current = current -> next;
	}
	printf("->NULL\n");
}

void free_list(VisionFrame **head){
	if (head == NULL){ return; }
	VisionFrame *current = *head;

	while(current != NULL){
		VisionFrame *next_node = current -> next;
		free(current);
		current = next_node;
	}

	*head = NULL;
}

//other function

int get_menu(void){
	int choice;
	printf(
	"========================================\n"
	"    ROBOT VISION LOG SYSTEM\n"
	"========================================\n"
	"1. Load Data from File\n"
	"2. Add Single Frame Manually\n"
	"3. Print All Logs (Screen & File)\n"
	"4. Filter Low Confidence Frames (< 0.5)\n"
	"5. Sort Frames by Confidence (Desc)\n"
	"6. Search by Category Keyword\n"
	"7. Show Statistics\n"
	"8. Check Memory Usage\n"
	"9. Clear All Data\n"
	"0. Exit\n"
	"========================================\n"
	"Enter choice: "	
	);
	scanf("%d", &choice);
	return choice;
}

//menu function
void clear_all(VisionFrame **head){
	free_list(head);
	printf("[+] All data cleared and memory freed.");
}
void exit_it(VisionFrame **head){
	clear_all(head);
	printf("Exiting program. Goodbye!");
}

//fuction7 statistics
void summary(const VisionFrame *head){
    if (head == NULL){
        printf("The list is empty.\n");
        return;
    }

    int total_frames = 0;
    int total_objects = 0;
    double total_confidence = 0.0, average_confidence = 0.0;
    int high_confidence_count = 0;

    const VisionFrame *current = head;
    while (current != NULL) {
        total_frames++;
        total_objects += current->data.target_count;
        if (current->data.target_count > 0){
            for (int i = 0; i < current->data.target_count; i++) {
                total_confidence += current->data.targets[i].conf;
                if (current->data.targets[i].conf > 0.8) {
                    high_confidence_count++;
                }
            }
        }
        current = current->next;       
    }
    if (total_objects > 0) {
        average_confidence = total_confidence / total_objects;
    } 
    else {
        average_confidence = 0.0;
    }
    printf("--- Statistics Summary ---\n");
    printf("%-25s : %d\n", "Total frames", total_frames);
    printf("%-25s : %d\n", "Total objects detected", total_objects);
    printf("%-25s : %.2f\n", "Global average confidence", average_confidence);
    printf("%-25s : %d\n", "High confidence count", high_confidence_count);
    printf("---------------------------\n");
}

//function8 memory usage monitor
void check_memory(const VisionFrame *head){
    if (head == NULL){
        printf("The list is empty.\n");
        return;
    }

    int active_nodes = 0;
    int size_per_node = sizeof(VisionFrame);
    int memory_used = 0;

    const VisionFrame *current = head;
    while (current != NULL) {
        active_nodes++;
        current = current->next;       
    }

    memory_used = active_nodes * size_per_node;
    printf("--- Memory Monitor ---\n");
    printf("%-18s : %d\n", "Active nodes", active_nodes);
    printf("%-18s : %d bytes\n", "Size per Node", size_per_node);
    printf("%-18s : %d bytes\n", "Total Heap Usage", memory_used);
    printf("----------------------\n");
}

double calculate_frame_avg_conf(const FrameData *data)
{
    if (data->target_count == 0)
        return 0.0;
    double sum = 0.0;
    for (int i = 0; i < data->target_count; i++)
    {
        sum += data->targets[i].conf;
    }
    return sum / data->target_count;
}

void print_all(const VisionFrame *head)
{
    if (head == NULL)
    {
        printf("[!] List is empty. Nothing to print.\n");
        return;
    }

    FILE *fp = fopen("report_output.txt", "w");
    if (fp == NULL)
    {
        fprintf(stderr, "Error: Failed to open report_output.txt for writing.\n");
        return;
    }


    const char *header = "========================================================================================\n"
                         "                                  VISION LOG REPORT\n"
                         "========================================================================================\n"
                         "ID   | Time                | Cam   | Targets | AvgConf | Details (Cat/Conf)\n"
                         "----------------------------------------------------------------------------------------\n";

    printf("%s", header);
    fprintf(fp, "%s", header);

    const VisionFrame *current = head;
    while (current != NULL)
    {
        double avg_conf = calculate_frame_avg_conf(&(current->data));


        char details[256] = "";
        for (int i = 0; i < current->data.target_count; i++)
        {
            char temp[50];
            snprintf(temp, sizeof(temp), "%s(%.2f)%s",
                     current->data.targets[i].category,
                     current->data.targets[i].conf,
                     (i == current->data.target_count - 1) ? "" : ", ");
            strcat(details, temp);
        }


        printf("%-4d | %-19s | %-5s | %-7d | %-7.2f | %s\n",
               current->data.frame_id,
               current->data.time_str,
               current->data.cam_id,
               current->data.target_count,
               avg_conf,
               details);

        fprintf(fp, "%-4d | %-19s | %-5s | %-7d | %-7.2f | %s\n",
                current->data.frame_id,
                current->data.time_str,
                current->data.cam_id,
                current->data.target_count,
                avg_conf,
                details);

        current = current->next;
    }

    const char *footer = "========================================================================================\n";
    printf("%s", footer);
    fprintf(fp, "%s", footer);

    fclose(fp);
    printf("[+] Report generated successfully: report_output.txt\n");
}


void filter(VisionFrame **head)
{
    if (head == NULL || *head == NULL)
    {
        printf("[!] List is empty. Nothing to filter.\n");
        return;
    }

    VisionFrame *current = *head;
    VisionFrame *prev = NULL;
    int removed_count = 0;

    while (current != NULL)
    {
        double avg_conf = calculate_frame_avg_conf(&(current->data));

        if (avg_conf < 0.5)
        {
            VisionFrame *temp = current;

            if (prev == NULL)
            {

                *head = current->next;
                current = *head;
            }
            else
            {

                prev->next = current->next;
                current = current->next;
            }

            free(temp); 
            removed_count++;
        }
        else
        {
            prev = current;
            current = current->next;
        }
    }

    printf("[+] Frames with average confidence < 0.50 removed (%d frame(s) deleted).\n", removed_count);
}
