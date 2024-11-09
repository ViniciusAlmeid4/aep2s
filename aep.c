#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <stdlib.h>

#define CHAR_SET "k3C7P-oBY&8sLmR6ZXg.Ni!E_fAt#4Uavjq2wbOD5H0lMd9nW$xchVzQJTrpKF1SGy?"
#define CHAR_SET_SIZE 68
#define chave 'A'

int findIndex(char ch)
{
    for (int i = 0; i < CHAR_SET_SIZE; i++)
    {
        if (CHAR_SET[i] == ch)
        {
            return i;
        }
    }
    return -1;
}

char *criptografa(const char *string)
{
    static char result[256];
    int stringSize = strlen(string);
    int chaveNum = findIndex(chave);
    if (chaveNum == -1)
    {
        chaveNum = 0;
    }
    result[0] = '\0';

    for (int count = 0; count < stringSize; count++)
    {
        int index = findIndex(string[count]);
        if (index != -1)
        {
            int newIndex = (index + chaveNum) % CHAR_SET_SIZE;
            strncat(result, &CHAR_SET[newIndex], 1);
        }
        else
        {
            strncat(result, &string[count], 1);
        }
    }
    return result;
}

char *descriptografa(const char *string)
{
    static char result[256];
    int stringSize = strlen(string);
    int chaveNum = findIndex(chave);
    if (chaveNum == -1)
    {
        chaveNum = 0;
    }
    result[0] = '\0';

    for (int count = 0; count < stringSize; count++)
    {
        int index = findIndex(string[count]);
        if (index != -1)
        {
            int newIndex = (index - chaveNum + CHAR_SET_SIZE) % CHAR_SET_SIZE;
            strncat(result, &CHAR_SET[newIndex], 1);
        }
        else
        {
            strncat(result, &string[count], 1);
        }
    }
    return result;
}

void listarUsuarios()
{
    FILE *arquivo = fopen("users.txt", "r");
    char linha[250], user[123], senha[123];
    char decryptedUser[123], decryptedSenha[123];

    if (arquivo == NULL)
    {
        printf("No users found!!");
        return;
    }
    
    while (fgets(linha, sizeof(linha), arquivo) != NULL)
    {
        sscanf(linha, "%122[^,], %122[^\n]", user, senha);

        strcpy(decryptedUser, descriptografa(user)); 
        strcpy(decryptedSenha, descriptografa(senha)); 

        printf("Usuário: %s || Senha: %s\n", decryptedUser, decryptedSenha);
    }
}



void cadastrarUsuario()
{
    FILE *arquivo = fopen("users.txt", "a+");
    char linha[250] = "", user[123], senha[123];
    printf("Digite qual o novo usuário: ");
    while (getchar() != '\n');
    fgets(user, 122, stdin);
    printf("Digite qual o nova senha: ");
    fgets(senha, 122, stdin);
    user[strcspn(user, "\n")] = '\0';
    senha[strcspn(senha, "\n")] = '\0';
    strcpy(linha, criptografa(user));
    strcat(linha, ", ");
    strcat(linha, criptografa(senha));
    fputs(linha, arquivo);
    fputs("\n", arquivo);
    fclose(arquivo);
}

void editarUsuario(const char *usernameToEdit)
{
    FILE *arquivo = fopen("users.txt", "r+");
    if (arquivo == NULL)
    {
        perror("Erro ao abrir o arquivo users.txt");
        return;
    }

    char linha[250], user[123], senha[123];
    long pos;
    int userFound = 0;

    while (fgets(linha, sizeof(linha), arquivo) != NULL)
    {
        sscanf(linha, "%122[^,], %122[^\n]", user, senha);

        if (strcmp(user, usernameToEdit) == 0)
        {
            userFound = 1;
            printf("Usuário encontrado: %s\n", user);
            printf("Digite o novo usuário: ");
            scanf(" %122s", user);
            printf("Digite a nova senha: ");
            scanf(" %122s", senha);

            pos = ftell(arquivo);
            fseek(arquivo, pos - strlen(linha) - 1, SEEK_SET);
            fprintf(arquivo, "%s, %s\n", criptografa(user), criptografa(senha));
            fflush(arquivo);
            break;
        }
    }

    fclose(arquivo);

    if (userFound)
    {
        printf("Usuário editado com sucesso!\n");
    }
    else
    {
        printf("Usuário não encontrado.\n");
    }
}

void excluirUsuario(const char *usernameToDelete)
{
    FILE *arquivo = fopen("users.txt", "r+");
    if (arquivo == NULL)
    {
        perror("Erro ao abrir o arquivo users.txt");
        return;
    }

    FILE *tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL)
    {
        perror("Erro ao criar o arquivo temp.txt");
        fclose(arquivo);
        return;
    }

    char linha[250], user[123], senha[123];
    int userFound = 0;

    while (fgets(linha, sizeof(linha), arquivo) != NULL)
    {
        sscanf(linha, "%122[^,], %122[^\n]", user, senha);

        if (strcmp(user, usernameToDelete) == 0)
        {
            userFound = 1;
            printf("Usuário %s excluído.\n", descriptografa(user));
            continue;
        }

        fputs(linha, tempFile);
    }

    fclose(arquivo);
    fclose(tempFile);

    if (userFound)
    {
        arquivo = fopen("users.txt", "w");
        if (arquivo == NULL)
        {
            perror("Erro ao reabrir o arquivo users.txt");
            return;
        }

        tempFile = fopen("temp.txt", "r");
        if (tempFile == NULL)
        {
            perror("Erro ao abrir o arquivo temp.txt");
            fclose(arquivo);
            return;
        }

        while (fgets(linha, sizeof(linha), tempFile) != NULL)
        {
            fputs(linha, arquivo);
        }

        fclose(arquivo);
        fclose(tempFile);
        printf("Arquivo atualizado com sucesso!\n");
    }
    else
    {
        printf("Usuário não encontrado.\n");
    }
}

//* Criptografar
//* char* textoCriptografado = criptografa(textoOriginal, chave);

//* Descriptografar
//* char* textoDescriptografado = descriptografa(textoCriptografado, chave);

int main()
{
    char username[123];
    int acao = 1;

    setlocale(LC_ALL, "");

    while (acao != 0)
    {
        printf("\n\nDigite qual operação desejada:\n1 - Listar usuários\n2 - Cadastrar novo usuário\n3 - Editar usuário\n4 - Excluir usuário\n0 - Sair do programa\n\n");
        scanf("%d", &acao);
        printf("\n");
        switch (acao)
        {
        case 0:
            printf("Programa encerrado!!");
            break;
        case 1:
            listarUsuarios();
            break;
        case 2:
            cadastrarUsuario();
            break;
        case 3:
            printf("Digite o usuário que deseja editar: ");
            scanf(" %122s", username);
            editarUsuario(criptografa(username));
            break;
        case 4:
            printf("Digite o usuário que deseja excluir: ");
            scanf(" %122s", username);
            excluirUsuario(criptografa(username));
            break;

        default:
            printf("Ação desconhecida!!");
            break;
        }
    }

    return 0;
}