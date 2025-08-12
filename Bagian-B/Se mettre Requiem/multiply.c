/*
 * I don't know why this code is f'in works, but it does.
 * File      : multiply.c
 * Author    : Nayekah
 * Date      : 2025-08-01
 * Modified  : To include input validation.
 */

// Includes
#include <stdio.h>
// #include <time.h>

// MACROS AND CONSTANTS
#define ll long long
#define MOD 998244353
#define ROOT 3
#define MAX_N 524288
#define MAX_LEN 1000005
#define BASE 10000
#define WIDTH 4
#define MOD_MINUS_1 998244352
#define MOD_MINUS_2 998244351

// logic bitwise
#define IS_EQUAL(a, b) (!((a) ^ (b)))
#define IS_NOT_ZERO(a) (!!(a))
#define IS_LESS(a, b) (((subtract((a), (b))) >> 63) & 1)
#define IS_GREATER(a, b) IS_LESS(b, a)
#define IS_GREATER_EQUAL(a, b) (!IS_LESS(a, b))

// Arithmetic and mod operations
ll add(ll a, ll b) { ll carry; add_loop: if (!!b) { carry = a & b; a = a ^ b; b = carry << 1; goto add_loop; } return a; }
ll subtract(ll a, ll b) { ll borrow; sub_loop: if (!!b) { borrow = (~a) & b; a = a ^ b; b = borrow << 1; goto sub_loop; } return a; }
ll mod_add(ll a, ll b) { ll res = add(a, b); if (IS_GREATER_EQUAL(res, MOD)) { res = subtract(res, MOD); } return res; }
ll mod_sub(ll a, ll b) { ll res = subtract(a, b); if (IS_LESS(res, 0)) { res = add(res, MOD); } return res; }
ll mod_mul(ll a, ll b) { ll res = 0; mul_loop: if (IS_GREATER(b, 0)) { if (b & 1) { res = mod_add(res, a); } a = mod_add(a, a); b = b >> 1; goto mul_loop; } return res; }
ll power(ll base, ll exp) { ll res = 1; pow_loop: if (IS_GREATER(exp, 0)) { if (exp & 1) { res = mod_mul(res, base); } base = mod_mul(base, base); exp = exp >> 1; goto pow_loop; } return res; }
ll modInverse(ll n) { return power(n, MOD_MINUS_2); }

// NTTs
// ======================================================================
static ll ntt_a[MAX_N], ntt_b[MAX_N];
static int rev[MAX_N];

void ntt(ll a[], int n, int invert) {
    int i = 0;
bit_rev_loop:
    if (IS_LESS(i, n)) {
        if (IS_LESS(i, rev[i])) { ll temp = a[i]; a[i] = a[rev[i]]; a[rev[i]] = temp; }
        i = add(i, 1);
        goto bit_rev_loop;
    }
    int len = 2, log_len = 1;
len_loop:
    if (IS_GREATER_EQUAL(n, len)) {
        ll wlen = power(ROOT, MOD_MINUS_1 >> log_len);
        if (IS_NOT_ZERO(invert)) { wlen = modInverse(wlen); }
        int j = 0;
    j_loop:
        if (IS_LESS(j, n)) {
            ll w = 1;
            int k = 0, half_len = len >> 1;
        k_loop:
            if (IS_LESS(k, half_len)) {
                int pos1 = add(j, k), pos2 = add(pos1, half_len);
                ll u = a[pos1], v = mod_mul(a[pos2], w);
                a[pos1] = mod_add(u, v);
                a[pos2] = mod_sub(u, v);
                w = mod_mul(w, wlen);
                k = add(k, 1);
                goto k_loop;
            }
            j = add(j, len);
            goto j_loop;
        }
        len = len << 1;
        log_len = add(log_len, 1);
        goto len_loop;
    }
    if (IS_NOT_ZERO(invert)) {
        ll n_inv = modInverse(n);
        i = 0;
    scale_loop:
        if (IS_LESS(i, n)) { a[i] = mod_mul(a[i], n_inv); i = add(i, 1); goto scale_loop; }
    }
}

// Main multiplication logic
// ======================================================================
static ll result[MAX_N];
static char input_str[MAX_LEN];

int read_to_base(ll arr[], const char* prompt) {
    printf("%s", prompt);
    scanf("%s", input_str);

    int i = 0;
check_loop:
    if (input_str[i] != '\0') {
        ll is_invalid_char = add(IS_LESS(input_str[i], '0'), IS_GREATER(input_str[i], '9'));

        if (IS_NOT_ZERO(is_invalid_char)) {
            printf("\nError: Invalid input.\n");
            printf("Only positive integers are allowed.\n");
            return -1;
        }
        
        i = add(i, 1);
        goto check_loop;
    }

    int len = i;

    int arr_idx = 0;
    int current_pos = len;
read_loop:
    if (IS_GREATER(current_pos, 0)) {
        int chunk_start = subtract(current_pos, WIDTH);
        if (IS_LESS(chunk_start, 0)) { chunk_start = 0; }
        
        ll val = 0;
        int j = chunk_start;
    chunk_loop:
        if (IS_LESS(j, current_pos)) {
            ll digit = subtract(input_str[j], '0');
            val = add(mod_mul(val, 10), digit);
            j = add(j, 1);
            goto chunk_loop;
        }
        arr[arr_idx] = val;
        arr_idx = add(arr_idx, 1);
        current_pos = subtract(current_pos, WIDTH);
        goto read_loop;
    }
    return arr_idx;
}

void print_from_base(ll arr[], int len) {
    printf("Result: ");
    printf("%lld", arr[subtract(len, 1)]);
    int i = subtract(len, 2);
print_loop:
    if (IS_GREATER_EQUAL(i, 0)) {
        //printf("%0*lld", WIDTH, arr[i]);
        
        if (IS_LESS(arr[i], 1000)) {
            printf("0");
        }
        if (IS_LESS(arr[i], 100)) {
            printf("0");
        }
        if (IS_LESS(arr[i], 10)) {
            printf("0");
        }

        printf("%lld", arr[i]);
        
        i = subtract(i, 1);
        goto print_loop;
    }
    printf("\n");
}

void multiply_convolution() {
    printf("zxcvbn calculator :3\n");
    printf("up to 10^1000000 integer size\n\n");
    
    int len_a = read_to_base(ntt_a, "a: ");

    if (IS_EQUAL(len_a, -1)) {
        return;
    }

    int len_b = read_to_base(ntt_b, "b: ");

    if (IS_EQUAL(len_b, -1)) {
        return;
    }
    
    printf("\nComputing multiplication (a * b)...\n");

    int a_is_zero = 0;
    if (IS_EQUAL(len_a, 1)) {
        if (IS_EQUAL(ntt_a[0], 0)) {
            a_is_zero = 1;
        }
    }
    if (a_is_zero) {
        printf("Result: 0\n");
        return;
    }

    int b_is_zero = 0;
    if (IS_EQUAL(len_b, 1)) {
        if (IS_EQUAL(ntt_b[0], 0)) {
            b_is_zero = 1;
        }
    }
    if (b_is_zero) {
        printf("Result: 0\n");
        return;
    }
    
    int n = 1;
    int required_len = add(len_a, len_b);
n_size_loop:
    if (IS_LESS(n, required_len)) { n = n << 1; goto n_size_loop; }
    
    int i = len_a;
pad_a_loop:
    if (IS_LESS(i, n)) { ntt_a[i] = 0; i = add(i, 1); goto pad_a_loop; }
    i = len_b;
pad_b_loop:
    if (IS_LESS(i, n)) { ntt_b[i] = 0; i = add(i, 1); goto pad_b_loop; }

    rev[0] = 0;
    i = 1;
precompute_rev_loop:
    if (IS_LESS(i, n)) {
        ll term2 = 0;
        if (i & 1) {
            term2 = n >> 1;
        }
        rev[i] = (rev[i >> 1] >> 1) | term2;
        i = add(i, 1);
        goto precompute_rev_loop;
    }

    ntt(ntt_a, n, 0);
    ntt(ntt_b, n, 0);

    i = 0;
pointwise_mul_loop:
    if (IS_LESS(i, n)) { ntt_a[i] = mod_mul(ntt_a[i], ntt_b[i]); i = add(i, 1); goto pointwise_mul_loop; }

    ntt(ntt_a, n, 1);
    
    ll carry = 0;
    i = 0;
carry_loop:
    if (IS_LESS(i, n)) {
        ll current_val = add(ntt_a[i], carry);
        ll q = 0, rem = 0, temp_val = current_val;
        int bit = 63;
    div_base_loop:
        if(IS_GREATER_EQUAL(bit, 0)){
            rem = rem << 1; rem = rem | ((temp_val >> bit) & 1);
            q = q << 1;
            if(IS_GREATER_EQUAL(rem, BASE)){ rem = subtract(rem, BASE); q = q | 1; }
            bit = subtract(bit, 1);
            goto div_base_loop;
        }
        result[i] = rem; carry = q;
        i = add(i, 1);
        goto carry_loop;
    }

    int final_len = n;
propagate_carry:
    if (IS_NOT_ZERO(carry)) {
        ll q = 0, rem = 0, temp_val = carry;
        int bit = 63;
    final_div_base_loop:
        if (IS_GREATER_EQUAL(bit, 0)) {
            rem = rem << 1; rem = rem | ((temp_val >> bit) & 1);
            q = q << 1;
            if (IS_GREATER_EQUAL(rem, BASE)) { rem = subtract(rem, BASE); q = q | 1; }
            bit = subtract(bit, 1);
            goto final_div_base_loop;
        }
        result[final_len] = rem; carry = q;
        final_len = add(final_len, 1);
        goto propagate_carry;
    }

final_len_check:
    if (IS_GREATER(final_len, 1)) {
        if (IS_EQUAL(result[subtract(final_len, 1)], 0)) {
            final_len = subtract(final_len, 1);
            goto final_len_check;
        }
    }
    
    print_from_base(result, final_len);
}

int main() {
    // clock_t start_time = clock();

    multiply_convolution();

    // clock_t end_time = clock();
    // double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    // printf("Time taken: %f seconds\n", time_taken);

    return 0;
}