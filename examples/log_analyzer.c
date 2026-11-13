#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include "allocator.h"

#define MAX_SERVICES 16
#define LINE_COUNT 50000

typedef struct {
    const char *service_name;
    uint32_t total_requests;
    uint32_t error_count;
    uint32_t total_latency_ms;
} service_stats_t;

static void generate_mock_log_line(size_t index, char *out_buf, size_t buf_size) {
    static const char *services[] = {
        "Auth-Service", "AUTH-SERVICE", "Billing-API",
        "billing-api", "Search-Worker", "Media-CDN"
    };

    static const char *levels[] = {
        "INFO", "INFO", "INFO", "WARN", "ERROR"
    };

    static const char *messages[] = {
        "request completed", "cache miss", "retrying upstream", "timeout exceeded"
    };

    const char *service = services[index % 6];
    const char *level = levels[index % 5];
    const char *message = messages[index % 4];
    uint32_t latency_ms = (uint32_t)(10 + ((index * 37) % 240));

    snprintf(out_buf, buf_size, 
            "2026-11-13T01:17:%02zuZ [%s] %s latency=%ums msg=\"%s\"", 
            index % 60, level, service, latency_ms, message);
}

static void parse_log_lines(arena_t *arena, service_stats_t *service_stats, size_t *service_count, const char *log_line) {
    arena_temp_t temp_arena = arena_temp_begin(arena);

    size_t line_length = strlen(log_line) + 1;
    char *log_line_ptr = arena_alloc_align(arena, line_length, sizeof(char));
    if (log_line_ptr == NULL) {
        arena_temp_end(&temp_arena);
        return;
    }
    strcpy(log_line_ptr, log_line);

    char *token = strtok(log_line_ptr, " ");
    token = strtok(NULL, " ");
    bool is_error = false;
    if (strcmp(token, "[ERROR]") == 0){
        is_error = true;
    }
    token = strtok(NULL, " ");
    char *raw_service_string = token;
    token = strtok(NULL, " ");
    int latency = 0;
    if (strncmp(token, "latency=", 8) == 0) {
        latency = atoi(token + 8);
    }

    char *temp_service_string = arena_alloc_align(arena, strlen(raw_service_string) + 1, sizeof(char));
    for (size_t i = 0; i < strlen(raw_service_string) + 1; i++) {
        temp_service_string[i] = tolower(raw_service_string[i]);
    }
    char lowercase_service_string[64];
    strcpy(lowercase_service_string, temp_service_string);
    arena_temp_end(&temp_arena);

    for (size_t i = 0; i < *service_count; i++) {
        if (strcmp(lowercase_service_string, service_stats[i].service_name) == 0) {
            service_stats[i].total_requests++;
            service_stats[i].total_latency_ms += latency;
            if (is_error) {
                service_stats[i].error_count++;
            }
            return;
        }
    }
    
    if (*service_count < MAX_SERVICES) {
        size_t perm_len = strlen(lowercase_service_string) + 1;
        char *permanent_string = arena_alloc_align(arena, perm_len, sizeof(char));
        if (permanent_string == NULL) {
            return;
        }
        strcpy(permanent_string, lowercase_service_string);

        size_t idx = *service_count;
        service_stats[idx].service_name = permanent_string;
        service_stats[idx].total_requests = 1;
        service_stats[idx].error_count = is_error ? 1 : 0;
        service_stats[idx].total_latency_ms = latency;
        (*service_count)++;
    }
}

int main (void) {

    arena_t arena = arena_create(16 * 1024);
    if (arena.buffer == NULL) {
        return 1;
    }

    service_stats_t *service_stats = arena_alloc_zero(&arena, MAX_SERVICES * sizeof(service_stats_t));
    if (service_stats == NULL) {
        return 1;
    }

    size_t service_count = 0;
    size_t total_raw_bytes = 0;

    char line_buf[256];
    for (size_t i = 0; i < LINE_COUNT; i++) {
        generate_mock_log_line(i, line_buf, sizeof(line_buf));
        total_raw_bytes += strlen(line_buf);
        parse_log_lines(&arena, service_stats, &service_count, line_buf);
    }

    double total_mb = (double)total_raw_bytes / (1024.0 * 1024.0);
    printf("Processed %d log lines (%.2f MB raw text) using a %zu-byte arena.\n\n",
            LINE_COUNT, total_mb, arena.capacity);

    printf("%-16s %10s %10s %14s\n", "Service", "Requests", "Errors", "Avg Latency");
    printf("----------------------------------------------------\n");

    for (size_t i = 0; i < service_count; i++) {
        double avg_latency = (service_stats[i].total_requests > 0)
            ? (double)service_stats[i].total_latency_ms / (double)service_stats[i].total_requests
            : 0.0;

        printf("%-16s %10u %10u %11.2f ms\n",
        service_stats[i].service_name,
        service_stats[i].total_requests,
        service_stats[i].error_count,
        avg_latency);
    }

    double pct_used = ((double)arena.offset / (double)arena.capacity) * 100.0;
    printf("\nArena persistent memory used: %zu / %zu bytes (%.2f%%)\n",
            arena.offset, arena.capacity, pct_used);

    arena_free(&arena);

    return 0;
}